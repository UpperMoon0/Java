# KeepAlive Service

The KeepAlive service is a C++ application that periodically pings a list of configured URLs to keep them from idling. It features a dynamic configuration that can be updated at runtime via a RESTful API, and it exposes an HTTP endpoint for health checks.

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

If the `config.json` file is not found, the service will start with an empty list of targets.

## API Endpoints

The service provides several API endpoints for managing the configuration and monitoring the service's health.

### Health Check

*   **URL**: `/health`
*   **Method**: `GET`
*   **Success Response**:
    *   **Code**: 200 OK
    *   **Content**: `OK`

### Configuration Management

#### Get Current Configuration
*   **URL**: `/config`
*   **Method**: `GET`
*   **Success Response**:
    *   **Code**: 200 OK
    *   **Content**: The current `config.json` as a JSON object.

#### Add a New Target
*   **URL**: `/config`
*   **Method**: `PUT`
*   **Body**: A JSON object representing the new target (e.g., `{"url": "http://example.com", "interval": 300}`).
*   **Success Response**:
    *   **Code**: 200 OK

#### Delete a Target
*   **URL**: `/config`
*   **Method**: `DELETE`
*   **Body**: A JSON object with the URL of the target to delete (e.g., `{"url": "http://example.com"}`).
*   **Success Response**:
    *   **Code**: 200 OK

## Building and Running with Docker

To build and run the KeepAlive service using Docker, follow these steps:

1.  **Build the Docker image:**

    ```sh
    docker build -t keepalive-service .
    ```

2.  **Run the Docker container:**

    Mount `config.json` from the Host
    This method is flexible as it allows you to change the configuration without rebuilding the image.

    ```sh
    docker run -d -p 8080:8080 \
      -v /path/to/your/config.json:/app/config.json \
      --name keepalive-container keepalive-service
    ```
    *Replace `/path/to/your/config.json` with the actual path on your host machine.*

3.  **Verify the health check:**

    You can now access the health check endpoint at `http://localhost:8080/health`.