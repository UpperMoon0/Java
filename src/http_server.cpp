#include "http_server.h"
#include "config_manager.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

HttpServer::HttpServer(ConfigManager& configManager, keep_alive::KeepAlive& keepAlive)
    : configManager(configManager), keepAlive(keepAlive) {
    svr.Get("/health", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("OK", "text/plain");
    });

    svr.Get("/config", [&](const httplib::Request &, httplib::Response &res) {
        res.set_content(configManager.getConfig().dump(), "application/json");
    });

    svr.Put("/config", [&](const httplib::Request &req, httplib::Response &res) {
        try {
            json body = json::parse(req.body);
            if (body.contains("url") && body.contains("interval")) {
                Target target = {body["url"], body["interval"]};
                configManager.addTarget(target);
                keepAlive.reload_config();
                res.set_content("Target added", "text/plain");
            } else {
                res.status = 400;
                res.set_content("Invalid target format", "text/plain");
            }
        } catch (const json::parse_error& e) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
        }
    });

    svr.Delete("/config", [&](const httplib::Request &req, httplib::Response &res) {
        try {
            json body = json::parse(req.body);
            if (body.contains("url")) {
                configManager.deleteTarget(body["url"]);
                keepAlive.reload_config();
                res.set_content("Target deleted", "text/plain");
            } else {
                res.status = 400;
                res.set_content("Missing 'url' field", "text/plain");
            }
        } catch (const json::parse_error& e) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
        }
    });
}

void HttpServer::start() {
    svr.listen("0.0.0.0", 8080);
}

void HttpServer::stop() {
    svr.stop();
}