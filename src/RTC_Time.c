
#include "RTC_Time.h"
#include "wifi_modules.h"
#include "colors.h"


static int valid_date(int day, int mon, int year);
static int compare_two_dates(int year1, int year2, int mon1, int mon2, int day1, int day2);
static int numberofleapyear(int startyear, int endyear);
// static int check_rtc_date_after_set(int current_date[]);
static int set_time(uint64_t sec, uint64_t usec);
// static int valid_time(int sec,int min,int hour);
// static void compare_two_time(int first_time[], int second_time[]);

//structure for storing start and end dates for calculating difference in days
struct date {
    int day, month, year;
};

void read_time(void){
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    int m;
    printf("Today's Date: %d ", tm.tm_mday);
    m = tm.tm_mon+1;
    switch(m)
    {
        case 1:
            printf("Jan, ");
            break;
        case 2:
            printf("Feb, ");
            break;
        case 3:
            printf("Mar, ");
            break;
        case 4:
            printf("Apr, ");
            break;
        case 5:
            printf("May, ");
            break;
        case 6:
            printf("June, ");
            break;
        case 7:
            printf("July, ");
            break;
        case 8:
            printf("Aug, ");
            break;
        case 9:
            printf("Sep, ");
            break;
        case 10:
            printf("Oct, ");
            break;
        case 11:
            printf("Nov, ");
            break;
        case 12:
            printf("Dec, ");
            break;
    }
    printf("%d", tm.tm_year+1900);
    printf("\nToday's Time: ");
    static int AM_PM_Indicator;
    if (tm.tm_hour < 12) {
			AM_PM_Indicator = 0;
		} else {
			AM_PM_Indicator = 1;
		}
		if (tm.tm_hour> 12) {
			tm.tm_hour -= 12;
		} else if (tm.tm_hour== 0) {
			tm.tm_hour = 12;
		}
        if(AM_PM_Indicator==0){
        printf("%d:%d:%d AM\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }else if(AM_PM_Indicator==1){
            printf("%d:%d:%d PM\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
}

void esp_rtc_init(void){
    time_t now;
    time(&now);
    setenv("TZ", "UTC+5", 1);  // Set timezone to Nepal Standard Time
    tzset();
    struct timeval tm_vl;
    int sreturn;
}

static int set_time(uint64_t sec, uint64_t usec){
    struct timeval tm_vl;
    int sreturn;
    tm_vl.tv_sec= sec;
    tm_vl.tv_usec= usec;
    printf("SECONDS: %lld\n", sec);
    sreturn=settimeofday(&tm_vl, NULL);
    if(sreturn==0){
        printf("Fast Charger:: Successfully Set time\n");
        return 1;
    }else if(sreturn==-1){
        printf("Error!! in set time\n");
        return -1;
    }
    return 0;
}

void rtc_set_date(int *date){
    static uint64_t current_sec;
    int temp_set_date[3];
    int temp_get_date[3];
    int temp_set_time[3];
    int temp_get_time[3];
    int current_date[7];
    read_rtc_date_time(current_date);
    // temp_get_date[0]=current_date[0];
    // temp_get_date[1]=current_date[1];
    // temp_get_date[2]=current_date[2];

    temp_set_date[0]=date[0];
    temp_set_date[1]=date[1];
    temp_set_date[2]=date[2];
    temp_set_time[0]=date[3];
    temp_set_time[1]=date[4];
    temp_set_time[2]=date[5];

    //for rgb value 
    calHour = temp_set_time[2];
    calMin = temp_set_time[1];
    calSec = temp_set_time[0];

    temp_get_time[0]=current_date[5];
    temp_get_time[1]=current_date[4];
    temp_get_time[2]=current_date[3];

    //**** always put time which is default to the esp32 module itself and check for SECONDS_OFFSET individually *****//
    temp_get_time[0] = 3;
    temp_get_time[1] = 0;
    temp_get_time[2] = 0;
    temp_get_date[0] = 1;
    temp_get_date[1] = 1;
    temp_get_date[2] = 1970;

    printf("SET TIM =%d:%d:%d\n",temp_set_time[2],temp_set_time[1],temp_set_time[0]);
    printf("GET TIM =%d:%d:%d\n",temp_get_time[2],temp_get_time[1],temp_get_time[0]);

    current_sec = check_date(temp_set_date,temp_get_date);
    current_sec += check_time(temp_get_time,temp_set_time);
    set_time(current_sec, 0);
}

void read_rtc_date_time(int *datetime){

    time_t now_rtc_time;
    now_rtc_time = time(NULL);                     // request for the current system time 
    struct tm tm = *localtime(&now_rtc_time);   // convert the current time to local time 
    datetime[0]=tm.tm_mday;
    datetime[1]=tm.tm_mon+1;
    datetime[2]=tm.tm_year + 1900;
    datetime[3]=tm.tm_hour;
    datetime[4]=tm.tm_min;
    datetime[5]=tm.tm_sec;
    // printf("HOUR: %d\n\n", datetime[3]);
    int m;
    printf("Today's Date: %d ", tm.tm_mday);
    m = tm.tm_mon+1;                              //  0 for jan and 11 for dec 
    switch(m)
    {
        case 1:
            printf("Jan, ");
            break;
        case 2:
            printf("Feb, ");
            break;
        case 3:
            printf("Mar, ");
            break;
        case 4:
            printf("Apr, ");
            break;
        case 5:
            printf("May, ");
            break;
        case 6:
            printf("June, ");
            break;
        case 7:
            printf("July, ");
            break;
        case 8:
            printf("Aug, ");
            break;
        case 9:
            printf("Sep, ");
            break;
        case 10:
            printf("Oct, ");
            break;
        case 11:
            printf("Nov, ");
            break;
        case 12:
            printf("Dec, ");
            break;
    }
    printf("%d", tm.tm_year + 1900);  // add the 1900 to get the default 

    printf("\nToday's Time: ");

     //for rgb value 
    calHour = tm.tm_hour;
    calMin = tm.tm_min;
    calSec = tm.tm_sec;

    if(tm.tm_hour>=12)
    {
        if(tm.tm_hour==12)
        {
            printf("12");
        }
        else
        {
          printf("%d", tm.tm_hour-12);
        }
        printf(":%d:%d PM", tm.tm_min, tm.tm_sec);
    }
    else{
        printf("%d:%d:%d AM", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        printf("\n");
}

void set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    set_alarm_hour = hour;
    set_alarm_min = min;
    set_alarm_sec = sec;
    printf("Alarm time set for = Hour: %d, Minute: %d, Seconds: %d\n\n", set_alarm_hour, set_alarm_min, set_alarm_sec);
}

uint8_t get_alarm()
{
    int time[3];
    read_rtc_date_time(time);
    get_alarm_hour = time[3];
    get_alarm_min = time[4];
    get_alarm_sec = time[5];
    // printf("Hour: %d, Minute: %d, Seconds: %d\n\n", get_alarm_hour, get_alarm_min, get_alarm_sec);

    if(set_alarm_hour == get_alarm_hour && set_alarm_min == get_alarm_min && (set_alarm_sec == get_alarm_sec || set_alarm_sec == get_alarm_sec + 1))
    {
        // printf("Alarm Set! Ready to restart the system!\n");
        return 1;
    }
    else
    {
        return 0;
    }
}

uint64_t check_time(int first_time[], int last_time[]){
    int sec1,min1,hour1,sec2,min2,hour2, diff_seconds,diff_minutes,diff_hours;
    uint64_t seconds=0;
    sec1=first_time[0];
    min1=first_time[1];
    hour1=first_time[2];
    sec2=last_time[0];
    min2=last_time[1];
    hour2=last_time[2];
    if(sec1 > sec2)
    {
        sec2 += 60;
        --min2;
    }
    if(min1 > min2)
    {
        min2 += 60;
        --hour2;
    }
    diff_seconds = sec2 - sec1;
    diff_minutes = min2 - min1;
    diff_hours = hour2 - hour1;
    seconds = diff_hours * 3600 + diff_minutes * 60 + diff_seconds;
    return seconds;
}

unsigned long int convertDateToDays(struct date date){
    unsigned long int totalDays;
    int numLeap = 0;
    int monthsAddFromYearStart[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int i;

    // First, calculate the number of leap year since year one (not including date's year).
    for(i = 1; i < date.year; i++)
        if((i % 4 == 0 && i % 100 != 0) || (i % 4 == 0 && i % 400 == 0))
            numLeap++;

    // If it is a leap year, as of March there has been an extra day.
    if((date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 4 == 0 && date.year % 400 == 0))
        for(i = 2; i < 12; i++)
            monthsAddFromYearStart[i]++;

    // (Year - 1) * 356 + a day per leap year + days totaling the previous months + days of this month
    totalDays = (date.year - 1) * 365 + numLeap + monthsAddFromYearStart[date.month - 1] + date.day;

    return totalDays;
}

uint64_t check_date(int first_date[], int second_date[]) {
	static char console_buffer[100];
	static int day1, mon1, year1, day2, mon2, year2;// total_number_of_days;
	// static int day_diff, mon_diff, year_diff;
    unsigned long int dateDifference;
    
	day2 = first_date[0];
	mon2 = first_date[1];
	year2 = first_date[2];
	day1 = second_date[0];
	mon1 = second_date[1];
	year1 =second_date[2];
	sprintf(console_buffer, "Fast Charger:: Today Date  %d/%d/%d \n", mon2, day2, year2);
	printf("%s",console_buffer);
	sprintf(console_buffer, "Fast Charger:: Default Date %d/%d/%d \n", mon1, day1, year1);
	printf("%s",console_buffer);

	if (!valid_date(day1, mon1, year1)) {
		sprintf(console_buffer, "Fast Charger:: Default Date %d/%d/%d date is invalid.\n", mon1, day1, year1);
		printf("%s",console_buffer);
		return -1;
	}

	if (!valid_date(day2, mon2, year2)) {
		sprintf(console_buffer, "Fast Charger:: Current Date %d/%d/%d date is invalid.\n", mon2, day2, year2);
		printf("%s",console_buffer);
		return -1;
	}
	if (!compare_two_dates(year1, year2, mon1, mon2, day1, day2)) {
		printf("Fast Charger:: The date not match the required format\n");
	}
	// if (day1 == day2 && mon1 == mon2 && year1 == year2) {
	// 	printf("Fast Cahrger::No need to save the date the default date and the current date is same\n");
	// 	return 0;
	// }
	sprintf(console_buffer, "Fast Cahrger:: number of the leap year between these two date=%d\n", numberofleapyear(year1, year2));
	printf("%s",console_buffer);

    struct date startDate = {day1, mon1, year1};
    struct date endDate = {day2, mon2, year2};

    //calculating the difference of total number of days between two dates
    dateDifference = convertDateToDays(endDate) - convertDateToDays(startDate);
    printf("Difference in days: %lu.\n", dateDifference);
	return ((dateDifference * 86400) + SECONDS_OFFSET); //returning total seconds in total days
}

static int valid_date(int day, int mon, int year) {
	int is_valid = 1, is_leap = 0;

	if (year >= 1800 && year <= 9999) {

		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
			is_leap = 1;
		}

		if (mon >= 1 && mon <= 12) {
			if (mon == 2) {
				if (is_leap && day == 29) {
					is_valid = 1;
				} else if (day > 28) {
					is_valid = 0;
				}
			} else if (mon == 4 || mon == 6 || mon == 9 || mon == 11) {
				if (day > 30) {
					is_valid = 0;
				}
			}

			else if (day > 31) {
				is_valid = 0;
			} else if (day == 0) {
				is_valid = 0;
			}
		}

		else {
			is_valid = 0;
		}

	} else {
		is_valid = 0;
	}
	if (day == 0 && year == 2000 && mon == 0)
		is_valid = 1;

	return is_valid;
}

static int compare_two_dates(int year1, int year2, int mon1, int mon2, int day1, int day2) {
	if (year1 == 2099) {
		return 1;
	}
	if (year2 > year1) {
		return 1;
	} else if (year2 < year1) {
		return 0;
	} else if (year2 == year1) {
		if (mon2 > mon1) {
			return 1;
		} else if (mon2 < mon1) {
			return 0;
		} else if (mon2 == mon1) {
			if (day2 >= day1) {
				return 1;
			} else {
				return 0;
			}
		}
	}
	return 1;
}

static int numberofleapyear(int startyear, int endyear) {
	int acc = 0;
	int startYear = startyear;
	int endYear = endyear;
	if (startYear < endYear) {
		while (1) {
			if ((startYear % 4 == 0 && startYear % 100 != 0) || startYear % 400 == 0)
				acc++;
			if (startYear + 1 == endYear)
				break;
			startYear++;
		}
	}
	return acc;
}

int64_t get_time_usec(void){
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
    return time_us;
}