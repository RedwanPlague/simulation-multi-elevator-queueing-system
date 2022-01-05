from random import random, randint
from math import log


def exponential(mean):
    return - mean * log(random())


class Simulator:
    def __init__(self, config):
        self.config = config
        self.time = 0
        self.del_time = 0
        self.elev_time = 0
        self.max_del = 0
        self.max_elev = 0
        self.que_len = 0
        self.que_time = 0
        self.max_que = 0
        self.que_total = 0
        self.remain = 0
        self.el_return = [0] * self.config.elevator_count
        self.stop = [0] * self.config.elevator_count
        self.operate = [0] * self.config.elevator_count
        self.first = [0] * self.config.elevator_count
        self.occupants = [0] * self.config.elevator_count
        self.el_del = [0] * self.config.elevator_count
        self.sel_vec = [0] * (self.config.floor_count + 1)
        self.flr_vec = [0] * (self.config.floor_count + 1)
        self.delivery = [0]
        self.wait = [0]
        self.between = [0]
        self.arrive = [0]
        self.floor = [0]
        self.elevator = [0]
        self.limit = 0
        self.que_first = 0
        self.start_que = 0
        self.queue = 0
        self.i = 0

    def available_elevator(self):
        # 5, 21
        for k in range(self.config.elevator_count):
            if self.el_return[k] <= self.time:
                return k
        return -1

    def highest_visited_floor(self):
        for m in range(self.config.floor_count, 0, -1):
            if self.sel_vec[m] > 0:
                return m
        return 0

    def load_new_customer(self):
        self.i += 1
        self.between.append(exponential(self.config.mean_interarrival_time))
        self.floor.append(randint(2, self.config.floor_count))
        self.arrive.append(0)
        self.wait.append(0)
        self.delivery.append(0)
        self.elevator.append(0)

    def load_elevator(self, j):
        # 6
        self.first[j] = self.i
        self.occupants[j] = 0
        self.sel_vec = [0] * len(self.sel_vec)
        self.flr_vec = [0] * len(self.flr_vec)
        while True:
            # 7
            self.sel_vec[self.floor[self.i]] = 1
            self.flr_vec[self.floor[self.i]] += 1
            self.occupants[j] += 1
            # 8
            self.load_new_customer()
            self.time += self.between[self.i]
            self.delivery[self.i] = self.config.door_holding_time
            # 9
            for k in range(self.config.elevator_count):
                self.el_return[k] = max(self.el_return[k], self.time)
            # 10
            if self.between[self.i] <= self.config.door_holding_time and \
                    self.occupants[j] < self.config.elevator_capacity:
                for k in range(self.first[j], self.i):
                    self.delivery[k] += self.between[self.i]
            else:
                self.limit = self.i - 1
                break

    def delivery_passengers(self, j):
        # 11
        for k in range(self.first[j], self.limit + 1):
            # 12
            n = self.floor[k]
            self.elevator[k] = (n - 1) * self.config.elevator_speed \
                + self.config.disembarking_time * sum(self.flr_vec[1:n]) \
                + self.config.disembarking_time \
                + (self.config.door_opening_time + self.config.door_closing_time) * sum(self.sel_vec[1:n]) \
                + self.config.door_opening_time
            # 13
            self.delivery[k] += self.elevator[k]
            # 14
            self.del_time += self.delivery[k]
            # 15
            self.max_del = max(self.max_del, self.delivery[k])
            # 16
            self.max_elev = max(self.max_elev, self.elevator[k])
            # 17
            self.stop[j] += sum(self.sel_vec[1:-1])
            mx = self.highest_visited_floor()
            self.el_del[j] = 2 * self.config.elevator_speed * (mx - 1) \
                + self.config.disembarking_time * sum(self.flr_vec[1:-1]) \
                + (self.config.door_opening_time + self.config.door_closing_time) * sum(self.sel_vec[1:-1])
            self.el_return[j] = self.time + self.el_del[j]
            self.operate[j] += self.el_del[j]

    def run(self):
        self.load_new_customer()
        self.delivery[self.i] = self.config.door_holding_time
        self.time = self.between[self.i]
        # 4
        while self.time <= self.config.simulation_termination_time:
            # 5
            j = self.available_elevator()
            if j != -1:
                # 6 - 10
                self.load_elevator(j)
                # 11 - 17
                self.delivery_passengers(j)
            else:
                # 19
                self.que_first = self.i
                self.start_que = self.time
                self.queue = 1
                self.arrive[self.i] = self.time
                while j == -1:
                    # 20
                    self.load_new_customer()
                    self.time += self.between[self.i]
                    self.arrive[self.i] = self.time
                    self.queue += 1
                    # 21
                    j = self.available_elevator()
                # 22
                self.sel_vec = [0] * len(self.sel_vec)
                self.flr_vec = [0] * len(self.flr_vec)
                remain = self.queue - self.config.elevator_capacity
                # 23
                if remain <= 0:  # queue is now empty
                    r = self.i
                    self.occupants[j] = self.queue
                else:
                    r = self.que_first + (self.config.elevator_capacity - 1)
                    self.occupants[j] = self.config.elevator_capacity
                # 24
                for k in range(self.que_first, r + 1):
                    self.sel_vec[self.floor[k]] = 1
                    self.flr_vec[self.floor[k]] += 1
                # 25
                self.que_len = max(self.que_len, self.queue)
                # 26
                self.que_total += self.occupants[j]
                self.que_time += sum(self.time - self.arrive[m] for m in range(self.que_first, r + 1))
                # 27
                self.max_que = max(self.max_que, self.time - self.start_que)
                # 28
                self.first[j] = self.que_first
                # 29
                for k in range(self.first[j], r + 1):
                    self.wait[k] = self.time - self.arrive[k]
                    self.delivery[k] = self.config.door_holding_time + self.wait[k]
                # 30
                if self.remain <= 0:
                    self.queue = 0



