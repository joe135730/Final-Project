import json
import time
import requests

SERVER = "http://127.0.0.1:5000"


def main():
    while True:
        try:
            resp = requests.get(f"{SERVER}/summary", timeout=1)
            resp.raise_for_status()
            data = resp.json()
            print(json.dumps(data, indent=2))
        except Exception as exc:
            print(f"Error fetching summary: {exc}")
        time.sleep(1)


if __name__ == "__main__":
    main()


