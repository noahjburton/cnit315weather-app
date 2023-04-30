#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "api.h"

#define MAX 100

struct WeatherData* cities[MAX]; // array that stores 100 cities' weather data
int end = 0; // stores the index value for the first unused element in the array

// function prototypes
void addCity(char time[30], char city[30], char country[30], int temp, int feelsTemp, int minTemp, int maxTemp, int wind, int percipitation);
int getCityIndex(char city[30]);

int main()
{
    wthr *test = (wthr *) malloc(sizeof(wthr));
    initialize(test, "New York");
    update(test);
    printwthr(test);

    return (0);
}

// add a city to end of the array, or update the city if already in the array
void addCity(char time[30], char city[30], char country[30], int temp, int feelsTemp, int minTemp, int maxTemp, int wind, int percipitation)
{
    // create and copy city 
    struct WeatherData* newCity = (struct WeatherData*)malloc(sizeof(struct WeatherData));
    strcpy(newCity->time, time);
	strcpy(newCity->city, city);
	strcpy(newCity->country, country);
	newCity->temp = temp;
    newCity->feelsTemp = feelsTemp;
    newCity->minTemp = minTemp;
    newCity->maxTemp = maxTemp;
    newCity->wind = wind;
    newCity->percipitation = percipitation;

    // check if city in already in the array
    int index = getCityIndex(city);
    // if not in the array at city to end of array
    if (index == -1)
    {
        cities[end] = newCity;
        end++;
    }
    // otherwise free the memory of the old city in the array, and update it
    else
    {
        free(cities[index]);
        cities[index] = newCity;
    }
}

// returns the index value of the city in the array, returns -1 if not found
int getCityIndex(char city[30])
{
    // traverse until the end of array
    for (int i = 0; i < end; i++)
    {
        // for each element compare if city string value is same
        if (strcmp(city,cities[i]->city)==0)
        {
            // return the index value if found
            return i;
        }
    }
    // not found then return -1
    return -1;
}

