#include "config_webpage.hpp"

#include <map>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include "Configs.hpp"

namespace {
	const std::map<const String, const EspSoftwareSerial::Config> serial_configs {
		{"8 bits de dado, sem paridade, 	1 bit de parada",	 EspSoftwareSerial::Config::SWSERIAL_8N1},
		{"8 bits de dado, paridade par, 	1 bit de parada",	 EspSoftwareSerial::Config::SWSERIAL_8E1},
		{"8 bits de dado, paridade ímpar, 1 bit de parada",	 EspSoftwareSerial::Config::SWSERIAL_8O1},
		{"8 bits de dado, sem paridade, 	2 bit de parada", EspSoftwareSerial::Config::SWSERIAL_8N2},
		{"8 bits de dado, paridade par, 	2 bit de parada", EspSoftwareSerial::Config::SWSERIAL_8E2},
		{"8 bits de dado, paridade ímpar, 2 bit de parada", EspSoftwareSerial::Config::SWSERIAL_8O2}
	};
}

void setup_config_webpage(AsyncWebServer* server, Configs* configs);

void save_and_set_new_baudrate(Configs* configs, AsyncWebServerRequest* request);
void save_and_set_new_serial_config(Configs* configs, AsyncWebServerRequest* request);

String config_page(Configs* configs);
String config_page_header();
String config_page_footer();
String config_rtu_html(Configs* configs);

void setup_config_webpage(AsyncWebServer* server, Configs* configs) {
	server->on("/configure", HTTP_GET, [configs](AsyncWebServerRequest* request) {
		request->send(200, "text/html", config_page(configs));
	});

	server->on("/configure", HTTP_POST, [configs](AsyncWebServerRequest* request) {
		save_and_set_new_baudrate(configs, request);
		save_and_set_new_serial_config(configs, request);
		request->send(200, "text/html", config_page(configs));
	});

}

void save_and_set_new_baudrate(Configs* configs, AsyncWebServerRequest* request)
{
	if (request->hasParam("rtu_baudrate", true)) {
		unsigned int new_baudrate = request->getParam("rtu_baudrate", true)->value().toInt();

		configs->set_rtu_baudrate(new_baudrate);
	}
}

void save_and_set_new_serial_config(Configs* configs, AsyncWebServerRequest* request)
{
	if (request->hasParam("rtu_serial_config", true)) {
		EspSoftwareSerial::Config new_config = static_cast<EspSoftwareSerial::Config>(request->getParam("rtu_serial_config", true)->value().toInt());

		configs->set_rtu_serial_config(new_config);
	}
}

String config_page(Configs* configs)
{
	String html;
	html += config_page_header();
	html += config_rtu_html(configs);
	html += config_page_footer();
	return html;
}

String config_page_header()
{
	return "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Configurar</title></head><body>";
}

String config_page_footer()
{
	return "</body></html>";
}

String config_rtu_html(Configs* configs)
{
	String html;
	html += "<h1>RTU</h1>";
	html += "<form action=\"/configure\" method=\"post\">";
	html += "<label for=\"rtu_baudrate\">Baudrate: </label>";
	html += "<input type=\"number\" id=\"rtu_baudrate\" name=\"rtu_baudrate\" value=\"" + String(configs->rtu_baudrate()) + "\">";
	html += "<br>";
	html += "<label for=\"rtu_serial_config\">Parâmetros: </label>";
	html += "<select id=\"rtu_serial_config\" name=\"rtu_serial_config\">";
	for (const auto& [description, config] : serial_configs) {
		html += "<option value=\"" + String(config) + "\"";
		if (config == configs->rtu_serial_config()) {
			html += " selected";
		}
		html += ">" + description + "</option>";
	}
	html += "</select>";
	html += "<br>";
	html += "<input type=\"submit\" value=\"Salvar\">";
	html += "</form>";
	return html;
}
