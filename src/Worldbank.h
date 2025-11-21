#ifndef WORLDBANK_H
#define WORLDBANK_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Worldbank {
private:
    std::string api_base = "https://api.worldbank.org/v2";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("api.worldbank.org"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    Worldbank(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> get_sources(int per_page=0,int page=1) {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        return make_api_call("/sources" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_countries(int per_page=0,int page=1,const std::string& region="",const std::string& incomeLevel="",const std::string& lendingType="") {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        if (!region.empty()) params["region"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(region)));
        if (!incomeLevel.empty()) params["incomeLevel"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(incomeLevel)));
        if (!lendingType.empty()) params["lendingType"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lendingType)));
        return make_api_call("/country" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_indicators(int per_page=0,int page=1,const std::string& source="") {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        if (!source.empty()) params["source"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(source)));
        return make_api_call("/indicator" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_country_indicator(const std::string& country_code,const std::string& indicator_code,int per_page=0,int page=1,const std::string& date="",int mrv=0) {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        if (!date.empty()) params["date"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(date)));
        if (mrv>0) params["mrv"] = std::to_string(mrv);
        return make_api_call("/country/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(country_code))) + "/indicator/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(indicator_code))) + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_indicator_values(const std::string& indicator_code,int per_page=0,int page=1,const std::string& date="",int mrv=0) {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        if (!date.empty()) params["date"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(date)));
        if (mrv>0) params["mrv"] = std::to_string(mrv);
        return make_api_call("/indicator/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(indicator_code))) + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_topic_list(int per_page=0,int page=1) {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        return make_api_call("/topic" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_region_list(int per_page=0,int page=1) {
        std::map<std::string, std::string> params;
        params["format"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t("json")));
        if (per_page>0) params["per_page"] = std::to_string(per_page);
        if (page>0) params["page"] = std::to_string(page);
        return make_api_call("/region" + build_query_params(params),"GET");
    }

};

#endif

