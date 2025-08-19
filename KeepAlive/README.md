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

    ```sh
    docker run -d -p 8080:8080 --name keepalive-container keepalive-service
    ```

    This command starts the container in detached mode and maps port `8080` of the container to port `8080` on the host.

3.  **Verify the health check:**

    You can now access the health check endpoint at `http://localhost:8080/health`.