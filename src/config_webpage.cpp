#include "config_webpage.hpp"

#include <map>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

#include "Configs.hpp"

#include "OppositeSerial.hpp"

namespace {
	enum UniSerialConfigForPrinting {
		S8N1, S8E1, S8O1, S8N2, S8E2, S8O2, ERROR
	};

	// use portuguese on webpage, even though debug is kept in english
	const std::map<const UniSerialConfigForPrinting, const String> webpage_config_str {
		{S8N1, "8 bits de dado, sem paridade, 	1 bit de parada"},
		{S8E1, "8 bits de dado, paridade par, 	1 bit de parada"},
		{S8O1, "8 bits de dado, paridade ímpar, 1 bit de parada"},
		{S8N2, "8 bits de dado, sem paridade, 	2 bit de parada"},
		{S8E2, "8 bits de dado, paridade par, 	2 bit de parada"},
		{S8O2, "8 bits de dado, paridade ímpar, 2 bit de parada"},
		{ERROR, "Erro: a configuração não está mapeada"}
	};

	// debug strings are kept in english because most libraries use english
	// on their logging
	const std::map<const UniSerialConfigForPrinting, const String> serial_dbg_config_str {
		{S8N1, "SWSERIAL_8N1"},
		{S8E1, "SWSERIAL_8E1"},
		{S8O1, "SWSERIAL_8O1"},
		{S8N2, "SWSERIAL_8N2"},
		{S8E2, "SWSERIAL_8E2"},
		{S8O2, "SWSERIAL_8O2"},
		{ERROR, "Error: config not mapped"}
	};

	// allow only standard baudrates
	const unsigned long serial_baudrates[] = {4800, 9600, 19200, 38400, 115200};

	String rtu_baudrate_param_name = "rtuBR";
	String rtu_serial_config_param_name = "rtuSC";
}

template <typename RTU, typename DBG>
void save_and_set_new_baudrate(Configs<RTU, DBG>* configs, AsyncWebServerRequest* request);
template <typename RTU, typename DBG>
void save_and_set_new_serial_config(Configs<RTU, DBG>* configs, AsyncWebServerRequest* request);

template <typename RTU, typename DBG>
String config_page(Configs<RTU, DBG>* configs);
String config_page_header();
String config_page_footer();
template <typename RTU, typename DBG>
String config_rtu_html(Configs<RTU, DBG>* configs);

UniSerialConfigForPrinting get_uni_serial_config_for_printing(SWSConfig config);
UniSerialConfigForPrinting get_uni_serial_config_for_printing(HWSConfig config);

template <typename SI>
void setup_config_webpage(AsyncWebServer* server, Configs<typename OppositeSerial<SI>::Type, SI>* configs, SI* dbg_serial)
{
	// TODO: find a way to redirect from / to /configure without breaking the POST request

	server->on("/configure", HTTP_GET, [configs](AsyncWebServerRequest* request) {
		request->send(200, "text/html", config_page(configs));
	});

	server->on("/configure", HTTP_POST, [configs, dbg_serial](AsyncWebServerRequest* request) {
		save_and_set_new_baudrate(configs, request);
		save_and_set_new_serial_config(configs, request);

		configs->update();

		dbg_serial->printf("Configs updated| RTU Baudrate: %d, RTU Serial Config: %s\r\n",
			configs->rtu_baudrate(), serial_dbg_config_str.at(
				get_uni_serial_config_for_printing(configs->rtu_serial_config())
			).c_str());

		request->send(200, "text/html", config_page(configs));
	});
}

template <typename RTU, typename DBG>
void save_and_set_new_baudrate(Configs<RTU, DBG>* configs, AsyncWebServerRequest* request)
{
	if (request->hasParam(rtu_baudrate_param_name, true)) {
		unsigned int new_baudrate = request->getParam(rtu_baudrate_param_name, true)->value().toInt();
		configs->set_rtu_baudrate(new_baudrate);
	}
}

template <typename RTU, typename DBG>
void save_and_set_new_serial_config(Configs<RTU, DBG>* configs, AsyncWebServerRequest* request)
{
	if (request->hasParam(rtu_serial_config_param_name, true)) {
		SConfigSelector_t<RTU> new_config =
			static_cast<SConfigSelector_t<RTU>>(request->getParam(rtu_serial_config_param_name, true)->value().toInt());

		configs->set_rtu_serial_config(new_config);
	}
}

template <typename RTU, typename DBG>
String config_page(Configs<RTU, DBG>* configs)
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

template <typename RTU, typename DBG>
String config_rtu_html(Configs<RTU, DBG>* configs)
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
	for (const auto& [config, description] : webpage_config_str) {
		html += "<option value=\"" + String(config) + "\"";
		if (config == get_uni_serial_config_for_printing(configs->rtu_serial_config())) {
			html += " selected";
		}
		html += ">" + description + "</option>";
	}
	html += "</select>";
	html += "<p>Parâmetro atual: " + webpage_config_str.at(
		get_uni_serial_config_for_printing(configs->rtu_serial_config())
	) + "</p>";

	html += "<input type=\"submit\" value=\"Salvar\">";
	html += "</form>";
	return html;
}

UniSerialConfigForPrinting get_uni_serial_config_for_printing(SWSConfig config)
{
	switch (config) {
	case SWSERIAL_8N1:
		return S8N1;
	case SWSERIAL_8E1:
		return S8E1;
	case SWSERIAL_8O1:
		return S8O1;
	case SWSERIAL_8N2:
		return S8N2;
	case SWSERIAL_8E2:
		return S8E2;
	case SWSERIAL_8O2:
		return S8O2;
	default: return ERROR;
	}
}

UniSerialConfigForPrinting get_uni_serial_config_for_printing(HWSConfig config)
{
	switch (config) {
	case SERIAL_8N1:
		return S8N1;
	case SERIAL_8E1:
		return S8E1;
	case SERIAL_8O1:
		return S8O1;
	case SERIAL_8N2:
		return S8N2;
	case SERIAL_8E2:
		return S8E2;
	case SERIAL_8O2:
		return S8O2;
	default: return ERROR;
	}
}
