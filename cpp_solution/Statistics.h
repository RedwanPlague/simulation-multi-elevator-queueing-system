#ifndef STATISTICS_H
#define STATISTICS_H

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

struct Statistics {
    int total_customers_served;
    double avg_delivery_time, max_delivery_time;
    double avg_time_in_elevator, max_time_in_elevator;
    int max_que_len;
    double avg_que_len, avg_time_in_que, max_time_in_que;
    vector<int> total_stops, max_load_count;
    vector<double> operation_time, idle_time, avg_load_size;

    Statistics operator+(const Statistics &s) const {
        Statistics ret = s;
        ret.total_customers_served += total_customers_served;
        ret.avg_que_len += avg_que_len;
        ret.max_que_len += max_que_len;
        ret.avg_time_in_que += avg_time_in_que;
        ret.max_time_in_que += max_time_in_que;
        ret.avg_time_in_elevator += avg_time_in_elevator;
        ret.max_time_in_elevator += max_time_in_elevator;
        ret.avg_delivery_time += avg_delivery_time;
        ret.max_delivery_time += max_delivery_time;
        for (int i = 0; i < (int)total_stops.size(); i++) {
            ret.total_stops[i] += total_stops[i];
        }
        for (int i = 0; i < (int)operation_time.size(); i++) {
            ret.operation_time[i] += operation_time[i];
        }
        for (int i = 0; i < (int)idle_time.size(); i++) {
            ret.idle_time[i] += idle_time[i];
        }
        for (int i = 0; i < (int)avg_load_size.size(); i++) {
            ret.avg_load_size[i] += avg_load_size[i];
        }
        for (int i = 0; i < (int)max_load_count.size(); i++) {
            ret.max_load_count[i] += max_load_count[i];
        }
        return ret;
    }

    Statistics operator/(double N) {
        Statistics ret = *this;
        ret.total_customers_served /= N;
        ret.avg_que_len /= N;
        ret.max_que_len /= N;
        ret.avg_time_in_que /= N;
        ret.max_time_in_que /= N;
        ret.avg_time_in_elevator /= N;
        ret.max_time_in_elevator /= N;
        ret.avg_delivery_time /= N;
        ret.max_delivery_time /= N;
        for (int i = 0; i < (int)total_stops.size(); i++) {
            ret.total_stops[i] /= N;
        }
        for (int i = 0; i < (int)operation_time.size(); i++) {
            ret.operation_time[i] /= N;
        }
        for (int i = 0; i < (int)idle_time.size(); i++) {
            ret.idle_time[i] /= N;
        }
        for (int i = 0; i < (int)avg_load_size.size(); i++) {
            ret.avg_load_size[i] /= N;
        }
        for (int i = 0; i < (int)max_load_count.size(); i++) {
            ret.max_load_count[i] /= N;
        }
        return ret;
    }

    static int print_headers(ostream &out, int N, bool empty = false) {
        vector<string> headers({"Tot Cust Srvd", "Avg Q Len", "Mx Q Len", "Avg Tm Q", "Mx Tm Q", "Avg Tm Elv",
                                "Mx Tm Elv", "Avg Dlvry Tm", "Mx Dlvry Tm"});
        for (string h : {"Op Tm", "Idl Tm", "Avg Ld", "No Mx Ld", "Tot Stp"}) {
            headers.push_back(h + " 1");
            for (int i = 2; i <= N; i++) {
                headers.push_back(to_string(i));
            }
        }

        for (auto &x : headers) {
            out << (empty ? string(x.size(), '-') : x) << ",";
        }
        out << endl;

        return headers.size();
    }

    void print_data(ostream &out) const {
        int prc = 0;
        out << fixed << setprecision(prc);
        out << total_customers_served << ",";
        out << fixed << setprecision(2) << avg_que_len << ",";
        out << fixed << setprecision(prc);
        out << max_que_len << ",";
        out << avg_time_in_que << ",";
        out << max_time_in_que << ",";
        out << avg_time_in_elevator << ",";
        out << max_time_in_elevator << ",";
        out << avg_delivery_time << ",";
        out << max_delivery_time << ",";
        for (auto &v : {operation_time, idle_time, avg_load_size}) {
            for (auto &x : v) {
                out << x << ",";
            }
        }
        for (auto &v : {max_load_count, total_stops}) {
            for (auto &x : v) {
                out << x << ",";
            }
        }
        out << endl;
    }
};

#endif // STATISTICS_H
