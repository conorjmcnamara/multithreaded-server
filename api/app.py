import os
from flask import Flask, request, jsonify
from dotenv import load_dotenv

app = Flask(__name__)

@app.route("/api/user", methods=["POST"])
def user():
    username = request.form["username"]
    return jsonify(f"Received: {username}")

if __name__ == "__main__":
    current_dir = os.path.dirname(os.path.abspath(__file__))
    dotenv_path = os.path.join(current_dir, "..", ".env")
    load_dotenv(dotenv_path)
    app.run(host=os.getenv("API_IP"), port=os.getenv("API_PORT"))