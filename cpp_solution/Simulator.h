#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <algorithm>
#include <cassert>
#include <chrono>
#include <random>
#include <vector>

#include "Config.h"
#include "Statistics.h"

using namespace std;

#define EPS 1e-20

#define all(v)     (v).begin(), (v).end()
#define str(var)   (#var)
#define align(var) setw(sizeof(#var) - 1) << var

default_random_engine arrival_gen, floor_gen, batch_gen;

void set_seed() {
    arrival_gen.seed(chrono::system_clock::now().time_since_epoch().count());
    floor_gen.seed(chrono::system_clock::now().time_since_epoch().count());
    batch_gen.seed(chrono::system_clock::now().time_since_epoch().count());
}

template <typename T> T sum(const vector<T> &v) {
    T ret = 0;
    for (auto &x : v) {
        ret += x;
    }
    return ret;
}

template <typename T> T sum(const vector<T> &v, int a, int b) {
    T ret = 0;
    for (int i = a; i <= b; i++) {
        ret += v[i];
    }
    return ret;
}

double divide(int a, int b) { return a / (b + EPS); }

int largest_non_zero_index(const vector<int> &v) {
    for (int i = v.size() - 1; i >= 0; i--) {
        if (v[i] != 0) {
            return i;
        }
    }
    return -1;
}

class Simulator {
    Config config;

    // current customer
    int i;

    // current elevator
    int j;

    // current time
    double time;

    int quelensum, quelen, quetotal, queue, quecust, remain, limit, N, Max, R;
    double deltime, elevtime, maxque, maxdel, maxelev, startque, quetime;

    // per elevator statistics
    vector<double> return_, operate, eldel;
    vector<int> stop, first, occup, total_load, max_load_count, load_count;
    vector<vector<int>> selvec, flrvec;

    // per customer statistics
    vector<double> delivery, wait, between, arrive, elevator;
    vector<int> floor;

    // random
    uniform_real_distribution<double> interarrival;
    uniform_int_distribution<int> random_floor;
    binomial_distribution<int> random_batch;

  public:
    Simulator(Config config) : config(config) {
        i = 0, time = 0;

        quelensum = quelen = quetotal = queue = quecust = remain = limit = N = Max = R = 0;
        deltime = elevtime = maxque = maxdel = maxelev = startque = quetime = 0;

        int E = config.elevator_count;
        int F = config.floor_count;

        // initialize per elevator statistics
        return_.resize(E, 0);
        operate.resize(E, 0);
        eldel.resize(E, 0);
        stop.resize(E, 0);
        first.resize(E, 0);
        occup.resize(E, 0);
        total_load.resize(E, 0);
        load_count.resize(E, 0);
        max_load_count.resize(E, 0);
        selvec.resize(E, vector<int>(F + 1, 0));
        flrvec.resize(E, vector<int>(F + 1, 0));

        // initialize per customer statistics
        delivery.resize(1, 0);
        wait.resize(1, 0);
        between.resize(1, 0);
        arrive.resize(1, 0);
        elevator.resize(1, 0);
        floor.resize(1, 0);

        interarrival = uniform_real_distribution<double>(0, 1);
        random_floor = uniform_int_distribution<int>(2, config.floor_count);
        random_batch = binomial_distribution<int>(config.batch_size - 1, 0.5);
    }

    void advance_time(double t) {
        time += t;
        quelensum += (t * queue);
    }

    void load_new_customer() {
        i++;
        if (i < (int)between.size()) {
            return;
        }

        int new_cust_count = 1 + random_batch(batch_gen);

        between.push_back(-config.mean_interarrival_time * log(interarrival(arrival_gen)));
        for (int k = 1; k < new_cust_count; k++) {
            between.push_back(0);
        }
        for (int k = 0; k < new_cust_count; k++) {
            floor.push_back(random_floor(floor_gen));
            delivery.push_back(0);
            arrive.push_back(0);
            wait.push_back(0);
            elevator.push_back(0);
        }
    }

    int available_elevator() {
        for (int k = 0; k < config.elevator_count; k++) {
            if (return_[k] <= time) {
                return k;
            }
        }
        return -1;
    }

    void do_steps_12_to_16(int k) {
        /* step_12: */
        N = floor[k] - 1;
        elevator[k] =
            (config.elevator_speed * N) + (config.disembarking_time * sum(flrvec[j], 1, N)) + config.disembarking_time +
            ((config.door_opening_time + config.door_closing_time) * sum(selvec[j], 1, N)) + config.door_opening_time;
        /* step_13: */
        delivery[k] += elevator[k];
        /* step_14: */
        deltime += delivery[k];
        /* step_15: */
        maxdel = max(maxdel, delivery[k]);
        /* step_16: */
        maxelev = max(maxelev, elevator[k]);
    }

    void do_step_17() {
        /* step_17: */
        stop[j] += sum(selvec[j]);
        Max = largest_non_zero_index(selvec[j]);
        eldel[j] = (2 * config.elevator_speed * (Max - 1)) + (config.disembarking_time * sum(flrvec[j])) +
                   ((config.door_opening_time + config.door_closing_time) * sum(selvec[j]));
        return_[j] = time + eldel[j];
        operate[j] += eldel[j];
        if (return_[j] > config.simulation_termination_time) {
            operate[j] -= (return_[j] - config.simulation_termination_time);
        }
    }

    void use_occup() {
        total_load[j] += occup[j];
        load_count[j]++;
        if (occup[j] == config.elevator_capacity) {
            max_load_count[j]++;
        }
    }

    Statistics run() {
        load_new_customer();
        delivery[i] = config.door_holding_time;
        /* step_3: */
        time = between[i];
        for (int k = 0; k < config.elevator_count; k++) {
            return_[k] = time;
            stop[k] = operate[k] = 0;
        }
        /* step_4: */
        while (time <= config.simulation_termination_time) {
        step_5:
            j = available_elevator();
            if (j == -1) {
                goto step_19;
            }
            /* step_6: */
            first[j] = i;
            occup[j] = 0;
            fill(all(selvec[j]), 0);
            fill(all(flrvec[j]), 0);
        step_7:
            selvec[j][floor[i]] = 1;
            flrvec[j][floor[i]]++;
            occup[j]++;
        step_8:
            load_new_customer();
            advance_time(between[i]);
            if (time > config.simulation_termination_time) {
                goto step_33;
            }
            delivery[i] = config.door_holding_time;
            /* step_9: */
            for (int k = 0; k < config.elevator_count; k++) {
                return_[k] = max(return_[k], time);
            }
            /* step_10: */
            if (between[i] <= config.door_holding_time && occup[j] < config.elevator_capacity) {
                for (int k = first[j]; k < i; k++) {
                    delivery[k] += between[i];
                }
                goto step_7;
            }
            else {
                limit = i - 1;
                goto step_11;
            }
        step_11:
            use_occup();
            for (int k = first[j]; k <= limit; k++) {
                do_steps_12_to_16(k);
            }
            do_step_17();
            /* step_18: */
            goto step_5;
        step_19:
            quecust = i;
            startque = time;
            queue = 1;
            arrive[i] = time;
        step_20:
            load_new_customer();
            advance_time(between[i]);
            if (time > config.simulation_termination_time) {
                goto step_33;
            }
            arrive[i] = time;
            queue++;
            /* step_21: */
            j = available_elevator();
            if (j == -1) {
                goto step_20;
            }
            /* step_22: */
            fill(all(selvec[j]), 0);
            fill(all(flrvec[j]), 0);
            remain = queue - config.elevator_capacity;
            /* step_23: */
            if (remain <= 0) {
                R = i;
                occup[j] = queue;
            }
            else {
                R = quecust + (config.elevator_capacity - 1);
                occup[j] = config.elevator_capacity;
            }
            use_occup();
            /* step_24: */
            for (int k = quecust; k <= R; k++) {
                selvec[j][floor[k]] = 1;
                flrvec[j][floor[k]]++;
            }
            /* step_25: */
            quelen = max(quelen, queue);
            /* step_26: */
            quetotal += occup[j];
            quetime += (R - quecust + 1) * time - sum(arrive, quecust, R);
            /* step_27: */
            maxque = max(maxque, time - startque);
            /* step_28: */
            first[j] = quecust;
            /* step_29: */
            for (int k = first[j]; k <= R; k++) {
                wait[k] = time - arrive[k];
                delivery[k] = config.door_holding_time + config.embarking_time + wait[k];
            }
            /* step_30: */
            if (remain <= 0) {
                queue = 0;
                goto step_8;
            }
            else {
                limit = R;
                for (int k = first[j]; k <= limit; k++) {
                    do_steps_12_to_16(k);
                }
                do_step_17();
                goto step_31;
            }
        step_31:
            assert(remain >= 0);
            queue = remain;
            quecust = R + 1;
            startque = arrive[R + 1];
            /* step_32: */
            goto step_20;
        }
    step_33:
        Statistics stats;
        stats.total_customers_served = i - 1 - queue;
        stats.avg_delivery_time = divide(deltime, stats.total_customers_served);
        stats.max_delivery_time = maxdel;
        /* step_34: */
        stats.avg_time_in_elevator = divide(sum(elevator, 1, limit), limit);
        stats.max_time_in_elevator = maxelev;
        /* step_35: */
        stats.avg_que_len = divide(quelensum, time);
        stats.max_que_len = quelen;
        stats.avg_time_in_que = divide(quetime, quetotal);
        stats.max_time_in_que = maxque;
        /* step_36: */
        stats.total_stops = stop;
        stats.operation_time = operate;
        stats.max_load_count = max_load_count;
        for (int k = 0; k < config.elevator_count; k++) {
            stats.idle_time.push_back(config.simulation_termination_time - operate[k]);
            stats.avg_load_size.push_back(divide(total_load[k], load_count[k]));
        }

        return stats;
    }
};

#endif // SIMULATOR_H
