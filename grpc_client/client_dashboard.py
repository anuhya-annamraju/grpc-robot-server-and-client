import os
import sys

import build_protos

# Run compilation first
build_protos.compile_protos()

# Add the generated folder to the Python path so internal gRPC imports don't break
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "generated")))

import grpc
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Import our freshly generated proto modules
import robot_control_api_pb2
import robot_control_api_pb2_grpc


class RobotDashboard:
    def __init__(self):
        # Set up a connection channel to your running C++ Server
        self.channel = grpc.insecure_channel("127.0.0.1:50051")
        self.stub = robot_control_api_pb2_grpc.RobotControlServicesStub(self.channel)

        # Request configuration data packet
        self.request = robot_control_api_pb2.BatteryLevelRequest()

        # Initiate the server streaming response channel iterator
        self.data_stream = self.stub.GetBatteryLevel(self.request)

        # In-memory storage arrays for rendering the plot data
        self.x_indices = []
        self.percentages = []
        self.frame_counter = 0

        # Initialize the plotting layout window
        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], "r-o", label="Battery Level")
        self.ax.legend(loc="upper left")
        self.ax.grid(True)

    def update_plot(self, frame):
        try:
            # Pull the next frame from the live network stream
            response = next(self.data_stream)

            self.frame_counter += 1
            self.x_indices.append(self.frame_counter)
            self.percentages.append(response.percentage)

            # Keep only the last 30 data points so the chart doesn't crowd out
            if len(self.x_indices) > 30:
                self.x_indices.pop(0)
                self.percentages.pop(0)

            # Update data arrays inside the plot line layout
            self.line.set_data(self.x_indices, self.percentages)

            # Dynamically recalculate axes boundaries based on current values
            self.ax.relim()
            self.ax.autoscale_view()

        except StopIteration:
            print("Stream closed cleanly by the remote server.")
        except grpc.RpcError as e:
            print(f"Network stream connection severed: {e.details()}")
            return

        return self.line,

    def run(self):
        # Tie the network step update frame directly into matplotlib's render tick engine
        self.ani = FuncAnimation(self.fig, self.update_plot, blit=False, interval=1000, cache_frame_data=False)
        plt.title("Live Robot Battery Monitor")
        plt.xlabel("Sample Count")
        plt.ylabel("Battery Percentage")
        plt.show()


if __name__ == "__main__":
    dashboard = RobotDashboard()
    dashboard.run()