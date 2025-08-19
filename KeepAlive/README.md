# KeepAlive Service

The KeepAlive service is a C++ application that periodically pings a list of configured URLs to ensure they are responsive. It also exposes an HTTP endpoint for health checks.

If the `config.json` file is not found, the keep-alive functionality will be disabled, but the health check endpoint will remain active.

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