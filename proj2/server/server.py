import os
import subprocess

from flask import Flask, jsonify, render_template, request, send_from_directory

app = Flask(__name__)

UPLOAD_DIR = "uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)


def handle_file_upload(file, command):
    try:
        file_path = os.path.join(UPLOAD_DIR, file.filename)
        file.save(file_path)

        file_name, file_extension = os.path.splitext(file.filename)

        encoded_file_path = os.path.join(
            UPLOAD_DIR, f"{file_name}.encoded{file_extension}"
        )
        decoded_file_path = os.path.join(
            UPLOAD_DIR, f"{file_name}.decoded{file_extension}"
        )

        result = subprocess.run(
            command.format(
                file_path=file_path,
                encoded_file_path=encoded_file_path,
                decoded_file_path=decoded_file_path,
            ),
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        if result.returncode == 0:
            output_lines = result.stdout.strip().split("\n")
            if len(output_lines) != 4:
                return (
                    jsonify({"status": "error", "error": "Unexpected output format"}),
                    500,
                )

            original_file_size = output_lines[0]
            encoded_file_size = output_lines[1]
            compression_rate = output_lines[2]
            psnr = output_lines[3]

            response = {
                "status": "success",
                "original_file_size": int(original_file_size),
                "encoded_file_size": int(encoded_file_size),
                "compression_rate": float(compression_rate),
            }

            if psnr != "inf":
                response["psnr"] = float(psnr)

            response["download_url"] = f"/download/{file_name}.decoded{file_extension}"

            # Remove temporary files
            os.remove(file_path)
            os.remove(encoded_file_path)

            return jsonify(response), 200
        else:
            return jsonify({"status": "error", "error": result.stderr.strip()}), 500

    except Exception as e:
        return jsonify({"status": "error", "error": str(e)}), 500


@app.route("/image", methods=["POST"])
def upload_image():
    if "file" not in request.files or "golomb_param" not in request.form:
        return (
            jsonify({"status": "error", "error": "File or golomb_param missing"}),
            400,
        )

    file = request.files["file"]
    if not file.filename.endswith(".ppm"):
        return jsonify({"status": "error", "error": "Invalid file extension, expected .ppm"}), 400

    golomb_param = request.form["golomb_param"]
    quant_level = request.form.get("quant_level", "0")

    command = f"./../bin/imageTest {{file_path}} {{encoded_file_path}} {{decoded_file_path}} {golomb_param} {quant_level}"
    return handle_file_upload(file, command)


@app.route("/video", methods=["POST"])
def upload_video():
    if "file" not in request.files or "golomb_param" not in request.form:
        return (
            jsonify({"status": "error", "error": "File or golomb_param missing"}),
            400,
        )

    file = request.files["file"]
    if not file.filename.endswith(".y4m"):
        return jsonify({"status": "error", "error": "Invalid file extension, expected .y4m"}), 400

    golomb_param = request.form["golomb_param"]
    quant_level = request.form.get("quant_level", "0")

    command = f"./../bin/videoTest {{file_path}} {{encoded_file_path}} {{decoded_file_path}} {golomb_param} {quant_level}"
    return handle_file_upload(file, command)


@app.route("/audio", methods=["POST"])
def upload_audio():
    if "file" not in request.files:
        return jsonify({"status": "error", "error": "File missing"}), 400

    file = request.files["file"]
    if not file.filename.endswith(".wav"):
        return jsonify({"status": "error", "error": "Invalid file extension, expected .wav"}), 400

    golomb_param = request.form.get("golomb_param", "")

    command = f"./../bin/audioTest {{file_path}} {{encoded_file_path}} {{decoded_file_path}} {golomb_param}"
    return handle_file_upload(file, command)


@app.route("/", methods=["GET"])
def upload_page():
    return render_template("upload.html")


@app.route("/assets/<filename>")
def serve_asset(filename):
    asset_folder = os.path.join(app.root_path, "assets")
    return send_from_directory(asset_folder, filename)


@app.route("/download/<filename>", methods=["GET"])
def download_file(filename):
    decoded_file_path = os.path.join(UPLOAD_DIR, filename)
    if os.path.exists(decoded_file_path):
        return send_from_directory(UPLOAD_DIR, filename, as_attachment=True)
    else:
        return jsonify({"status": "error", "error": "File not found"}), 404


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
