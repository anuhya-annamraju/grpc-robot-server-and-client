import os
import sys
import queue
import threading
import matplotlib

# Prefer a non-Tkinter backend on macOS, then Qt-based backends.
for backend in ["MacOSX", "Qt5Agg", "QtAgg"]:
    try:
        matplotlib.use(backend, force=True)
        break
    except Exception:
        continue

import matplotlib.pyplot as plt
from matplotlib.widgets import Button
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "generated")))

import grpc
import robot_control_api_pb2
import robot_control_api_pb2_grpc


class RobotDashboard:
    def __init__(self):
        self.time_data = []
        self.pos_x_data = []
        self.pos_y_data = []
        self.pos_z_data = []
        self.counter = 0
        self.current_pos = (0.0, 0.0, 0.0)
        self.position_history = [self.current_pos]
        self.status_text = "Status: Ready"
        self.robot_id = "Robot ID: --"
        self.battery_text = "Battery: -- %"
        self.channel = grpc.insecure_channel("127.0.0.1:50051")
        self.stub = robot_control_api_pb2_grpc.RobotControlServicesStub(self.channel)
        self.command_queue = queue.Queue()
        self.telemetry_thread = None
        self.battery_thread = None
        self._build_ui()
        self._start_background_tasks()

    def _build_ui(self):
        self.fig = plt.figure(figsize=(11, 6), constrained_layout=True)
        gs = self.fig.add_gridspec(6, 4)

        self.ax_plot = self.fig.add_subplot(gs[:, :3], projection="3d")
        self.ax_status = self.fig.add_subplot(gs[0, 3])
        self.ax_button_forward = self.fig.add_subplot(gs[1, 3])
        self.ax_button_left = self.fig.add_subplot(gs[2, 3])
        self.ax_button_right = self.fig.add_subplot(gs[3, 3])
        self.ax_button_back = self.fig.add_subplot(gs[4, 3])
        self.ax_close = self.fig.add_subplot(gs[5, 3])

        self.ax_plot.set_title("Object Position (X, Y, Z)")
        self.ax_plot.set_xlabel("X")
        self.ax_plot.set_ylabel("Y")
        self.ax_plot.set_zlabel("Z")
        self.ax_plot.grid(True)
        self.ax_plot.set_xlim(-10.0, 10.0)
        self.ax_plot.set_ylim(-10.0, 10.0)
        self.ax_plot.set_zlim(-10.0, 10.0)
        self.ax_plot.view_init(elev=22, azim=45)

        self._refresh_plot()

        self.ax_status.axis("off")
        self.ax_status.set_position([0.8, 0.8, 0.2, 0.2])
        self.status_text_obj = self.ax_status.text(
            0.0,
            0.85,
            self.status_text,
            transform=self.ax_status.transAxes,
            fontsize=12,
            va="top",
            bbox={"boxstyle": "round,pad=0.45", "facecolor": "#f7f7f7", "edgecolor": "#cccccc"},
        )
        self.robot_id_text_obj = self.ax_status.text(0.0, 0.62, self.robot_id, transform=self.ax_status.transAxes, fontsize=11)
        self.battery_text_obj = self.ax_status.text(0.0, 0.48, self.battery_text, transform=self.ax_status.transAxes, fontsize=11)
        self.ax_status.text(0.0, 0.34, "Telemetry: waiting", transform=self.ax_status.transAxes, fontsize=11)

        button_kwargs = {"color": "#dddddd", "hovercolor": "#bbbbbb"}
        self.btn_forward = Button(self.ax_button_forward, "Forward ⬆", **button_kwargs)
        self.btn_left = Button(self.ax_button_left, "Left ⬅", **button_kwargs)
        self.btn_right = Button(self.ax_button_right, "Right ➡", **button_kwargs)
        self.btn_back = Button(self.ax_button_back, "Back ⬇", **button_kwargs)
        self.btn_close = Button(self.ax_close, "Close Window", color="#f4b4b4", hovercolor="#e88b8b")

        self.btn_forward.label.set_fontsize(12)
        self.btn_left.label.set_fontsize(12)
        self.btn_right.label.set_fontsize(12)
        self.btn_back.label.set_fontsize(12)
        self.btn_close.label.set_fontsize(11)

        self.btn_forward.on_clicked(lambda event: self.queue_move_command("FORWARD"))
        self.btn_left.on_clicked(lambda event: self.queue_move_command("LEFT"))
        self.btn_right.on_clicked(lambda event: self.queue_move_command("RIGHT"))
        self.btn_back.on_clicked(lambda event: self.queue_move_command("BACK"))
        self.btn_close.on_clicked(lambda event: plt.close(self.fig))

        self.ax_button_forward.patch.set_facecolor("#f3f3f3")
        self.ax_button_left.patch.set_facecolor("#f3f3f3")
        self.ax_button_right.patch.set_facecolor("#f3f3f3")
        self.ax_button_back.patch.set_facecolor("#f3f3f3")
        self.ax_button_forward.patch.set_edgecolor("#999999")
        self.ax_button_left.patch.set_edgecolor("#999999")
        self.ax_button_right.patch.set_edgecolor("#999999")
        self.ax_button_back.patch.set_edgecolor("#999999")
        self.ax_button_forward.patch.set_linewidth(1)
        self.ax_button_left.patch.set_linewidth(1)
        self.ax_button_right.patch.set_linewidth(1)
        self.ax_button_back.patch.set_linewidth(1)

        self.ax_button_forward.axis("off")
        self.ax_button_left.axis("off")
        self.ax_button_right.axis("off")
        self.ax_button_back.axis("off")
        self.ax_close.axis("off")

    def _start_background_tasks(self):
        self._fetch_robot_id()
        self._start_battery_stream()
        self._start_telemetry_stream()

    def _fetch_robot_id(self):
        thread = threading.Thread(target=self._robot_id_worker, daemon=True)
        thread.start()

    def _robot_id_worker(self):
        try:
            response = self.stub.GetRobotId(robot_control_api_pb2.RobotIdRequest())
            self.robot_id = f"Robot ID: {response.id}"
            self.robot_id_text_obj.set_text(self.robot_id)
            self.fig.canvas.draw_idle()
        except grpc.RpcError as exc:
            print(f"Robot ID error: {exc}")
            self.robot_id = "Robot ID: unavailable"
            self.robot_id_text_obj.set_text(self.robot_id)
            self.fig.canvas.draw_idle()

    def _start_battery_stream(self):
        if self.battery_thread and self.battery_thread.is_alive():
            return
        self.battery_thread = threading.Thread(target=self._battery_worker, daemon=True)
        self.battery_thread.start()

    def _battery_worker(self):
        try:
            responses = self.stub.GetBatteryLevel(robot_control_api_pb2.BatteryLevelRequest())
            for response in responses:
                self.battery_text = f"Battery: {response.percentage} %"
                self.battery_text_obj.set_text(self.battery_text)
                self.fig.canvas.draw_idle()
        except grpc.RpcError as exc:
            print(f"Battery stream error: {exc}")
        finally:
            self.battery_thread = None

    def _start_telemetry_stream(self):
        if self.telemetry_thread and self.telemetry_thread.is_alive():
            return
        self.telemetry_thread = threading.Thread(target=self._telemetry_worker, daemon=True)
        self.telemetry_thread.start()

    def _telemetry_worker(self):
        try:
            responses = self.stub.MoveRobotCommand(self.command_generator())
            for response in responses:
                self._handle_telemetry_response(response)
        except grpc.RpcError as exc:
            print(f"Telemetry stream error: {exc}")
        finally:
            self.telemetry_thread = None

    def _refresh_plot(self):
        self.ax_plot.clear()
        self.ax_plot.set_title("Object Position (X, Y, Z)")
        self.ax_plot.set_xlabel("X")
        self.ax_plot.set_ylabel("Y")
        self.ax_plot.set_zlabel("Z")
        self.ax_plot.grid(True)
        self.ax_plot.set_xlim(-10.0, 10.0)
        self.ax_plot.set_ylim(-10.0, 10.0)
        self.ax_plot.set_zlim(-10.0, 10.0)
        self.ax_plot.view_init(elev=22, azim=45)

        if len(self.position_history) >= 2:
            trail_x = [point[0] for point in self.position_history]
            trail_y = [point[1] for point in self.position_history]
            trail_z = [point[2] for point in self.position_history]
            self.ax_plot.plot(trail_x, trail_y, trail_z, color="green", linewidth=1.5, alpha=0.8, label="Trail")

        x, y, z = self.current_pos
        self.ax_plot.scatter([x], [y], [z], s=140, color="royalblue", label="Object")
        self.ax_plot.text(x + 0.2, y + 0.2, z + 0.2, f"z={z:.2f}", fontsize=9)
        self.ax_plot.legend(loc="upper left")
        self.fig.canvas.draw_idle()

    def _handle_telemetry_response(self, response):
        print(f"Telemetry -> x={response.pos_x:.3f}, y={response.pos_y:.3f}, z={response.pos_z:.3f}")
        self.current_pos = (response.pos_x, response.pos_y, response.pos_z)
        self.position_history.append(self.current_pos)
        if len(self.position_history) > 80:
            self.position_history.pop(0)

        self.counter += 1
        self.time_data.append(self.counter)
        self.pos_x_data.append(response.pos_x)
        self.pos_y_data.append(response.pos_y)
        self.pos_z_data.append(response.pos_z)

        if len(self.time_data) > 50:
            self.time_data.pop(0)
            self.pos_x_data.pop(0)
            self.pos_y_data.pop(0)
            self.pos_z_data.pop(0)

        self._refresh_plot()

    def command_generator(self):
        while True:
            cmd = self.command_queue.get()
            yield cmd

    def queue_move_command(self, direction_str):
        if direction_str == "FORWARD":
            command = robot_control_api_pb2.MoveCommand(direction=robot_control_api_pb2.MoveCommand.Direction.FORWARD)
        elif direction_str == "LEFT":
            command = robot_control_api_pb2.MoveCommand(direction=robot_control_api_pb2.MoveCommand.Direction.LEFT)
        elif direction_str == "RIGHT":
            command = robot_control_api_pb2.MoveCommand(direction=robot_control_api_pb2.MoveCommand.Direction.RIGHT)
        elif direction_str == "BACK":
            command = robot_control_api_pb2.MoveCommand(direction=robot_control_api_pb2.MoveCommand.Direction.BACK)
        else:
            return

        self.command_queue.put(command)
        self.status_text = f"Command: {direction_str}"
        self.status_text_obj.set_text(self.status_text)
        self.fig.canvas.draw_idle()
        print(self.status_text)

    def show(self):
        plt.show()


if __name__ == "__main__":
    dashboard = RobotDashboard()
    dashboard.show()
