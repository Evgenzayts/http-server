//
// Created by ubuntu on 5/20/22.
//

#ifndef INCLUDE_SUGGESTION_HPP_
#define INCLUDE_SUGGESTION_HPP_

#include <nlohmann/json.hpp>
#include <boost/beast.hpp>
#include <boost/program_options.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <shared_mutex>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

class InfoJson {
 private:
  const std::string _path_suggestions = "/v1/api/suggest.json";
  json _all_suggest;

 public:
  void LoadInfo() {
      std::ifstream file(_path_suggestions);
      file >> _all_suggest;

      SortJson();
  }

  [[nodiscard]] json GetJson() const {
    std::cout << _all_suggest[0].at("id");
    return _all_suggest;
  }

 private:
  void SortJson() {
    std::sort(_all_suggest.begin(), _all_suggest.end(),
              [](const json& a, const json& b) -> bool {
                return a.at("cost") < b.at("cost");
              } );
  }
};

class Suggestion {
 private:
  json _all_suggest;
  json _response_suggest;

 public:
  explicit Suggestion(json info_json);

  json GetSuggest(json& input_json);

 private:
  void MakeSuggest(json& input_suggest);
};

#endif  // INCLUDE_SUGGESTION_HPP_
