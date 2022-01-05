from config import Config
from simulator import Simulator


def main():
    config = Config('input.txt')
    for _ in range(10):
        simulator = Simulator(config)
        simulator.run()


if __name__ == '__main__':
    main()
