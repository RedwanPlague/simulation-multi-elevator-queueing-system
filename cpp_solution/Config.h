#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <string>

using namespace std;

struct Config {
    int floor_count, elevator_count, batch_size, elevator_capacity;
    double simulation_termination_time;
    double elevator_speed;
    double door_holding_time, door_opening_time, door_closing_time;
    double embarking_time, disembarking_time;
    double mean_interarrival_time;

    Config(const string file_name) {
        ifstream fin(file_name);
        fin >> simulation_termination_time;
        fin >> floor_count >> elevator_count >> elevator_capacity >> batch_size;
        fin >> door_holding_time >> elevator_speed >> door_opening_time >> door_closing_time;
        fin >> embarking_time >> disembarking_time;
        fin >> mean_interarrival_time;
        mean_interarrival_time *= 60;
        fin.close();
    }
};

#endif // CONFIG_H
