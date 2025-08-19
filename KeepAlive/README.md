# KeepAlive Service

The KeepAlive service is a C++ application that periodically pings a list of configured URLs to ensure they are responsive. It also exposes an HTTP endpoint for health checks.

If the `config.json` file is not found, the keep-alive functionality will be disabled, but the health check endpoint will remain active.

## Configuration (`config.json`)

The `config.json` file defines the URLs that the service will ping. It contains a single key, `targets`, which is an array of objects. Each object in the array represents a target to be pinged and must have the following keys:

*   `url` (string): The URL to be pinged.
*   `interval` (integer): The interval in seconds at which to ping the URL.

**Example `config.json`:**
```json
{
    "targets": [
        {
            "url": "https://www.google.com",
            "interval": 60
        },
        {
            "url": "https://www.github.com",
            "interval": 120
        }
    ]
}
```

## Health Check

The service includes a health check endpoint to monitor its status.

*   **URL**: `/health`
*   **Method**: `GET`
*   **Success Response**:
    *   **Code**: 200 OK
    *   **Content**: `OK`

## Building and Running with Docker

To build and run the KeepAlive service using Docker, follow these steps:

1.  **Build the Docker image:**

    ```sh
    docker build -t keepalive-service .
    ```

2.  **Run the Docker container:**

    To run the container, you can choose one of two methods for providing the `config.json` file.

    **Option A: Mount `config.json` from the Host (Recommended)**

    This method is flexible as it allows you to change the configuration without rebuilding the image.

    ```sh
    docker run -d -p 8080:8080 \
      -v /path/to/your/config.json:/app/config.json \
      --name keepalive-container keepalive-service
    ```
    *Replace `/path/to/your/config.json` with the actual path on your host machine.*

    **Option B: Use the `config.json` Baked into the Image**

    If your configuration is static, you can use the `config.json` file that was copied into the image during the build.

    ```sh
    docker run -d -p 8080:8080 --name keepalive-container keepalive-service
    ```

3.  **Verify the health check:**

    You can now access the health check endpoint at `http://localhost:8080/health`.