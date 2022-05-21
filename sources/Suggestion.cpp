// Copyright 2022 Evgenzayts evgenzaytsev2002@yandex.ru

#include "Suggestion.hpp"

Suggestion::Suggestion(json info_json) {
  _all_suggest = info_json;
}

nlohmann::json Suggestion::GetSuggest(json& input_json) {
  MakeSuggest(input_json);

  return _response_suggest;
}

void Suggestion::MakeSuggest(json& input_suggest) {
  _response_suggest["suggestions"] = {};

  boost::optional<std::string> input_str_obj =
      input_suggest.at("input").get<std::string>();
  if (!input_str_obj.has_value()) {
    throw std::runtime_error("Empty input");
  }
  std::string input_str = input_str_obj.value();

  for (size_t i = 0, k = 0; i < _all_suggest.size(); ++i) {
    if (input_str == _all_suggest[i].at("id")) {
      json suggest_record;
      suggest_record["text"] = _all_suggest[i].at("name");
      suggest_record["position"] = ++k;
      _response_suggest["suggestions"].push_back(suggest_record);
    }
  }
}
