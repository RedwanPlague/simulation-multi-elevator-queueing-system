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
    double avg_time_in_que, max_time_in_que;
    vector<int> total_stops;
    vector<double> operation_percentage;

    Statistics operator+(const Statistics &s) const {
        Statistics ret = s;
        ret.total_customers_served += total_customers_served;
        ret.avg_delivery_time += avg_delivery_time;
        ret.max_delivery_time += max_delivery_time;
        ret.avg_time_in_elevator += avg_time_in_elevator;
        ret.max_time_in_elevator += max_time_in_elevator;
        ret.max_que_len += max_que_len;
        ret.avg_time_in_que += avg_time_in_que;
        ret.max_time_in_que += max_time_in_que;
        for (int i = 0; i < (int)total_stops.size(); i++) {
            ret.total_stops[i] += total_stops[i];
        }
        for (int i = 0; i < (int)operation_percentage.size(); i++) {
            ret.operation_percentage[i] += operation_percentage[i];
        }
        return ret;
    }

    Statistics operator/(double N) {
        Statistics ret = *this;
        ret.total_customers_served /= N;
        ret.avg_delivery_time /= N;
        ret.max_delivery_time /= N;
        ret.avg_time_in_elevator /= N;
        ret.max_time_in_elevator /= N;
        ret.max_que_len /= N;
        ret.avg_time_in_que /= N;
        ret.max_time_in_que /= N;
        for (int i = 0; i < (int)total_stops.size(); i++) {
            ret.total_stops[i] /= N;
        }
        for (int i = 0; i < (int)operation_percentage.size(); i++) {
            ret.operation_percentage[i] /= N;
        }
        return ret;
    }

    static void print_headers(ostream &out, int N, bool empty = false) {
        vector<string> headers({"Tot Cust Srvd", "Avg Dlvry Tm", "Max Dlvry Tm", "Avg Tm Elv", "Max Tm Elv",
                                "Max Q Len", "Avg Tm Q", "Max Tm Q"});
        for (int i = 1; i <= N; i++) {
            headers.push_back("Tot Stp " + to_string(i));
        }
        for (int i = 1; i <= N; i++) {
            headers.push_back("\% Op " + to_string(i));
        }

        for (auto &x : headers) {
            out << (empty ? string(x.size(), '-') : x) << ",";
        }
        out << endl;
    }

    void print_data(ostream &out) const {
        out << fixed << setprecision(0);
        out << total_customers_served << ",";
        out << avg_delivery_time << ",";
        out << max_delivery_time << ",";
        out << avg_time_in_elevator << ",";
        out << max_time_in_elevator << ",";
        out << max_que_len << ",";
        out << avg_time_in_que << ",";
        out << max_time_in_que << ",";
        for (auto &x : total_stops) {
            out << x << ",";
        }
        for (auto &x : operation_percentage) {
            out << x << ",";
        }
        out << endl;
    }
};

#endif // STATISTICS_H
