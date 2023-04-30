// stores weather data for current time and place
typedef struct WeatherData {
    char time[30];              
    char city[30];
    char country[30];
    int temp;
    int feelsTemp;
    int minTemp;
    int maxTemp;
    int wind;
    int percipitation;
    //double lat, lon;
} wthr;

void printwthr(wthr *);

void initialize(wthr *, char []);

void update(wthr *);
