/* See LICENSE file for copyright and license details. */
#include <stddef.h>

#include "../util.h"


#if defined(__linux__)
	#include <stdint.h>
	#include <string.h>
	#include <dirent.h>
	#include <limits.h>

	static const char HWMON[] = "/sys/class/hwmon";

	void
	temp(char *out, const char *unused)
	{
		DIR *d;
		char name[16];
		uintmax_t temp;
		struct dirent *dp;
		static char file[PATH_MAX] = {0};

		if (file[0])
			goto get_temp;

		if (!(d = opendir(HWMON))) {
			warn("opendir '%s':", HWMON);
			ERRRET(out);
		}

		while ((dp = readdir(d))) {
			if (!strcmp(dp->d_name, ".") ||
					!strcmp(dp->d_name, ".."))
				continue;

			esnprintf(file, sizeof(file), "%s/%s/%s",
					HWMON, dp->d_name, "name");

			if (pscanf(file, "%s", name) != 1) {
				warn("scanf '%s':", file);
				file[0] = '\0';
				continue;
			}

			if (!strcmp(name, "coretemp") ||
					!strcmp(name, "acpitz") ||
					!strcmp(name, "k10temp") ||
					!strcmp(name, "fam15h_power")) {
				esnprintf(file, sizeof(file),
						"%s/%s/%s", HWMON,
						dp->d_name, "temp1_input");
				break;
			}
		}
		closedir(d);
get_temp:
		if (pscanf(file, "%ju", &temp) != 1)
			ERRRET(out);
		bprintf(out, "%ju", temp / 1000);
	}
#elif defined(__OpenBSD__)
	#include <stdio.h>
	#include <sys/time.h> /* before <sys/sensors.h> for struct timeval */
	#include <sys/sysctl.h>
	#include <sys/sensors.h>

	void
	temp(char *out, const char *unused)
	{
		int mib[5];
		size_t size;
		struct sensor temp;

		mib[0] = CTL_HW;
		mib[1] = HW_SENSORS;
		mib[2] = 0; /* cpu0 */
		mib[3] = SENSOR_TEMP;
		mib[4] = 0; /* temp0 */

		size = sizeof(temp);

		if (sysctl(mib, 5, &temp, &size, NULL, 0) < 0) {
			warn("sysctl 'SENSOR_TEMP':");
			ERRRET(out);
		}

		/* kelvin to celsius */
		bprintf(out, "%d", (temp.value - 273150000) / 1E6);
	}
#elif defined(__FreeBSD__)
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/sysctl.h>

	void
	temp(char *out, const char *zone)
	{
		int temp;
		size_t len;
		char buf[256];

		len = sizeof(temp);
		snprintf(buf, sizeof(buf),
				"hw.acpi.thermal.%s.temperature", zone);
		if (sysctlbyname(buf, &temp, &len, NULL, 0) == -1 || !len)
			ERRRET(out);

		/* kelvin to decimal celcius */
		bprintf(out, "%d.%d", (temp - 2731) / 10,
				abs((temp - 2731) % 10));
	}
#endif
