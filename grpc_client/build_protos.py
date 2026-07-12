import os
import sys
from grpc_tools import protoc

def compile_protos():
    current_dir = os.path.abspath(os.path.dirname(__file__))
    print(f"Compiling .proto files in {current_dir}...")
    proto_dir = os.path.abspath(os.path.join(current_dir, "../proto_definitions"))
    
    # Target output folder inside grpc_client
    output_dir = os.path.join(current_dir, "generated")
    
    # Create the folder and its __init__.py if missing
    os.makedirs(output_dir, exist_ok=True)
    with open(os.path.join(output_dir, "__init__.py"), "a"): 
        pass

    proto_file = os.path.join(proto_dir, "robot_control_api.proto")

    proto_args = [
        "grpc_tools.protoc",
        f"-I{proto_dir}",
        f"--python_out={output_dir}",
        f"--grpc_python_out={output_dir}",
        proto_file,
    ]

    if protoc.main(proto_args) != 0:
        print("Error: Proto compilation failed.")
        sys.exit(1)

if __name__ == "__main__":
    compile_protos()