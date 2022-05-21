//
// Created by ubuntu on 5/20/22.
//
#include "Suggestion.hpp"

Suggestion::Suggestion(std::shared_ptr<InfoJson>& info_json) {
  SortSuggest(info_json->GetJson());
}

nlohmann::json Suggestion::GetSuggest(json& input_json) {
  MakeSuggest(input_json);

  return _response_suggest;
}

void Suggestion::MakeSuggest(json& input_suggest) {
  _response_suggest["suggestions"] = "";

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
void Suggestion::SortSuggest(json json_for_sort) {
  std::sort(json_for_sort.begin(), json_for_sort.end(),
            [](const json& a, const json& b) -> bool {
              return a.at("cost") < b.at("cost");
            });
  _all_suggest = json_for_sort;
}
