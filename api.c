#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "api.h"
#include <time.h>

#define APIBUFFERSIZE 4096
#define APIKEY "bdb104f070adad8ea6c9171c3de0a5e4"



char * call_api(char *); // returns a pointer to buffer where api response is written
size_t curl_writefunction(char *, size_t, size_t, void *); // write function for cURL
void json_prep(char *, size_t); // pass in api response buffer, isolates json and overwrites the rest with \0
void parse_insert(wthr *, char *); // parse JSON in char *buffer and insert values into data structure of type wthr
char * format_timestamp(time_t); // format time from UNIX timestamp
double kelvin_to_fahrenheit(double);
void printwthr(wthr *);

/*
int main() {

    wthr *test = (wthr *) malloc(sizeof(wthr));
    initialize(test, "New York");
    printwthr(test);
    
    return 0;

}
*/

char * call_api(char cityname[]) { // returns a pointer to buffer where response is written

    CURL *curl = curl_easy_init();

    char urlfmt[] = "https://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", url[256], *city_enc = curl_easy_escape(curl, cityname, 0);

    snprintf(url, sizeof(url), urlfmt, city_enc, APIKEY);

    //printf("%s", url);

    curl_easy_setopt(curl, CURLOPT_URL, url);

    char *buffer = (char *) malloc(APIBUFFERSIZE);
    if (buffer == NULL) printf("Response buffer malloc failed");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writefunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, APIBUFFERSIZE);

    CURLcode res = curl_easy_perform(curl);
    //printf("\n\n");

    if (res == CURLE_OK) ;//printf("curl_easy_peform completed\n");
    else printf("Error: %s\n", curl_easy_strerror(res));

    curl_free(city_enc);
    curl_easy_cleanup(curl);

    return buffer;

}

size_t curl_writefunction(char *ptr, size_t size, size_t nmemb, void *userdata) {

    size_t realsize = size * nmemb; // size of recieved data = size of each memeber * number of received members

    char *buffer = (char *)userdata; // point to buffer

    memcpy(buffer, ptr, realsize); // overwrite buffer with newly received data

    return realsize; // return size of data written to buffer
}

void json_prep(char *buffer, size_t buffer_size) {

    char *start = strchr(buffer, '{');
    char *end = strrchr(buffer, '}');
    
    if (start && end) {
        size_t json_length = (end + 1) - start;
        
        if (json_length < buffer_size) {
            memset(buffer + json_length, '\0', buffer_size - json_length);
        }
    }
}

void parse_insert(wthr *data, char *buffer) {

    cJSON *root = cJSON_Parse(buffer);

    strcpy(data->time, format_timestamp(cJSON_GetObjectItem(root, "dt")->valueint));
    strcpy(data->city, cJSON_GetObjectItem(root, "name")->valuestring);

    cJSON *sys = cJSON_GetObjectItem(root, "sys");

    strcpy(data->country, cJSON_GetObjectItem(sys, "country")->valuestring);

    cJSON *main = cJSON_GetObjectItem(root, "main");

    data->temp = kelvin_to_fahrenheit(cJSON_GetObjectItem(main, "temp")->valuedouble);
    data->feelsTemp = kelvin_to_fahrenheit(cJSON_GetObjectItem(main, "feels_like")->valuedouble);
    data->minTemp = kelvin_to_fahrenheit(cJSON_GetObjectItem(main, "temp_min")->valuedouble);
    data->maxTemp = kelvin_to_fahrenheit(cJSON_GetObjectItem(main, "temp_max")->valuedouble);

    cJSON *wind = cJSON_GetObjectItem(root, "wind");

    data->wind = cJSON_GetObjectItem(wind, "speed")->valuedouble;

    cJSON *rain = cJSON_GetObjectItem(root, "rain");

    if (cJSON_GetObjectItem(rain, "1h") == NULL)
        data->percipitation = 0;
    else
        data->percipitation = cJSON_GetObjectItem(rain, "1h")->valuedouble;

    cJSON_free(rain);
    cJSON_free(wind);
    cJSON_free(main);
    cJSON_free(sys);
    cJSON_free(root);

}

char * format_timestamp(time_t timestamp) {

    char* formatted_time = (char*) malloc(sizeof(char) * 20);
    struct tm* local_time = localtime(&timestamp);
    strftime(formatted_time, 20, "%I:%M %p", local_time);
    return formatted_time;

}

double kelvin_to_fahrenheit(double kelvin) {
    return (kelvin - 273.15) * 9.0/5.0 + 32.0;
}

void printwthr(wthr *test) {

    printf("Time: %s\n", test->time);
    printf("City: %s\n", test->city);
    printf("Country: %s\n", test->country);
    printf("Temp: %d\n", test->temp);
    printf("Feels Like: %d\n", test->feelsTemp);
    printf("Low: %d\n", test->minTemp);
    printf("High: %d\n", test->maxTemp);
    printf("Wind: %d\n", test->wind);
    printf("Precipitation: %d\n", test->percipitation);

}

void initialize(wthr *data, char city[]) {

    strcpy(data->city, city);
    update(data);

}

void update(wthr *data) {

    char *res = call_api(data->city);
    json_prep(res, APIBUFFERSIZE);
    parse_insert(data, res);
    free(res);

}