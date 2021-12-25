class Config:
    def __init__(self, input_file_name):
        with open(input_file_name, 'r') as f:
            self.simulation_termination_time = int(f.readline())
            self.floor_count, self.elevator_count, self.elevator_capacity, self.batch_size\
                = map(int, f.readline().split())
            self.door_holding_time, self.elevator_speed, self.door_opening_time, self.door_closing_time\
                = map(int, f.readline().split())
            self.embarking_time, self.disembarking_time = map(int, f.readline().split())
            self.mean_interarrival_time = int(float(f.readline()) * 60)
