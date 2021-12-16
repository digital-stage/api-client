//
// Created by Tobias Hegemann on 24.02.21.
//
#include "DigitalStage/Auth/AuthService.h"
#include <teckos/rest.h>                  // for Result, Header, rest
#include <iostream>                       // for string, operator<<, char_tr...
#include <map>                            // for operator!=, map
#include <nlohmann/json_fwd.hpp>          // for json
#include <string>                         // for basic_string, operator+
#include "DigitalStage/Auth/AuthError.h"  // for Auth, AuthError

using namespace DigitalStage::Auth;

std::future<bool> AuthService::verifyToken(const std::string &token) {
  return std::async(std::launch::async, [this, &token] {
    return verifyTokenSync(token);
  });
}

[[maybe_unused]] bool AuthService::verifyTokenSync(const std::string &token) {
  auto header = teckos::Header();
  header.insert({"Authorization", "Bearer " + token});
  auto result = teckos::rest::Get(this->url_ + "/profile");
  if (result.statusCode == 200) {
    return true;
  }
  std::cerr << "Could not verify token, reason is " << result.statusCode << ": " << result.statusMessage << std::endl;
  return false;
}

AuthService::AuthService(const std::string &authUrl) {
  this->url_ = authUrl;
}

std::future<std::string> AuthService::signIn(const std::string &email,
                                                            const std::string &password) {
  return std::async(std::launch::async, [this, &email, &password] {
    return signInSync(email, password);
  });
}

std::string AuthService::signInSync(const std::string &email,
                                    const std::string &password) {
  nlohmann::json jsonBody;
  jsonBody["email"] = email;
  jsonBody["password"] = password;
  auto header = teckos::Header();
  auto result = teckos::rest::Post(this->url_ + "/login", header, jsonBody);
  if (result.statusCode == 200) {
    return result.body.get<std::string>();
  }
  throw AuthError(result.statusCode, result.statusMessage);
}

std::future<bool> AuthService::signOut(const std::string &token) {
  return std::async(std::launch::async, [this, &token] {
    return signOutSync(token);
  });
}

[[maybe_unused]] bool AuthService::signOutSync(const std::string &token) {
  auto header = teckos::Header();
  header.insert({"Authorization", "Bearer " + token});
  auto result = teckos::rest::Post(this->url_ + "/logout", header);
  if (result.statusCode == 200) {
    return true;
  }
  std::cerr << "Could not sign out, reason is " << result.statusCode << ": " << result.statusMessage << std::endl;
  return false;
}