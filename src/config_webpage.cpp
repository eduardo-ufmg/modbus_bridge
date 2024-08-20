#include "config_webpage.hpp"

#include <map>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include "Configs.hpp"

namespace {
	const std::map<const EspSoftwareSerial::Config, const String> swserial_configs {
		{SWSERIAL_8N1, "8 bits de dado, sem paridade, 	1 bit de parada"},
		{SWSERIAL_8E1, "8 bits de dado, paridade par, 	1 bit de parada"},
		{SWSERIAL_8O1, "8 bits de dado, paridade ímpar, 1 bit de parada"},
		{SWSERIAL_8N2, "8 bits de dado, sem paridade, 	2 bit de parada"},
		{SWSERIAL_8E2, "8 bits de dado, paridade par, 	2 bit de parada"},
		{SWSERIAL_8O2, "8 bits de dado, paridade ímpar, 2 bit de parada"}
	};

	const unsigned long serial_baudrates[] = {4800, 9600, 19200, 38400, 115200};

	String rtu_baudrate_param_name = "rtuBR";
	String rtu_serial_config_param_name = "rtuSC";
}

void save_and_set_new_baudrate(Configs* configs, AsyncWebServerRequest* request);
void save_and_set_new_serial_config(Configs* configs, AsyncWebServerRequest* request);

String config_page(Configs* configs);
String config_page_header();
String config_page_footer();
String config_rtu_html(Configs* configs);

void setup_config_webpage(AsyncWebServer* server, Configs* configs, HardwareSerial* dbg_serial) {
	server->on("/configure", HTTP_GET, [configs](AsyncWebServerRequest* request) {
		request->send(200, "text/html", config_page(configs));
	});

	server->on("/configure", HTTP_POST, [configs, dbg_serial](AsyncWebServerRequest* request) {
		dbg_serial->printf("New baudrate: %lu\r\n", request->getParam(rtu_baudrate_param_name, true)->value().toInt());
		save_and_set_new_baudrate(configs, request);

		dbg_serial->printf("New serial config: %s\r\n",
			swserial_configs.at(static_cast<EspSoftwareSerial::Config>
				(request->getParam(rtu_serial_config_param_name, true)->value().toInt())).c_str());
		save_and_set_new_serial_config(configs, request);

		configs->update();
		dbg_serial->printf("Configs updated| RTU Baudrate: %d, RTU Serial Config: %s\r\n",
			configs->rtu_baudrate(), swserial_configs.at(configs->rtu_serial_config()).c_str());

		request->send(200, "text/html", config_page(configs));
	});

}

void save_and_set_new_baudrate(Configs* configs, AsyncWebServerRequest* request)
{
	if (request->hasParam(rtu_baudrate_param_name, true)) {
		unsigned int new_baudrate = request->getParam(rtu_baudrate_param_name, true)->value().toInt();
		configs->set_rtu_baudrate(new_baudrate);
	}
}

void save_and_set_new_serial_config(Configs* configs, AsyncWebServerRequest* request)
{
	if (request->hasParam(rtu_serial_config_param_name, true)) {
		EspSoftwareSerial::Config new_config =
			static_cast<EspSoftwareSerial::Config>(request->getParam(rtu_serial_config_param_name, true)->value().toInt());

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

	html += "<label for=\"rtuBR\">Baudrate: </label>";
	html += "<select id=\"rtuBR\" name=\"" + rtu_baudrate_param_name +"\">";
	for (const auto& baudrate : serial_baudrates) {
		html += "<option value=\"" + String(baudrate) + "\"";
		if (baudrate == configs->rtu_baudrate()) {
			html += " selected";
		}
		html += ">" + String(baudrate) + "</option>";
	}
	html += "</select>";
	html += "<p>Valor atual: " + String(configs->rtu_baudrate()) + "</p>";

	html += "<label for=\"rtuSC\">Parâmetros: </label>";
	html += "<select id=\"rtuSC\" name=\"" + rtu_serial_config_param_name +"\">";
	for (const auto& [config, description] : swserial_configs) {
		html += "<option value=\"" + String(config) + "\"";
		if (config == configs->rtu_serial_config()) {
			html += " selected";
		}
		html += ">" + description + "</option>";
	}
	html += "</select>";
	html += "<p>Parâmetro atual: " + swserial_configs.at(configs->rtu_serial_config()) + "</p>";

	html += "<input type=\"submit\" value=\"Salvar\">";
	html += "</form>";
	return html;
}
