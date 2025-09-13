#ifndef ENDPOINT_HANDLER_H
#define ENDPOINT_HANDLER_H

#include <functional>
#include <optional>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>


#include "Protocols/HTTP/RouterCommon.h"
      
namespace usub::server::protocols::http {
   /**
   * @class HTTPEndpointHandler
   * @brief Handles the registration and execution of HTTP endpoint routes and their associated middleware.
   *
   * The `HTTPEndpointHandler` class manages a collection of routes, allowing for the addition of new routes
   * and middleware. It matches incoming HTTP requests to the appropriate route and executes the corresponding
   * middleware and handler functions.
   *
   * @see Route
   * @see MiddlewareChain
   */
   class HTTPEndpointHandler {
   private:
      /**
      * @brief Vector of all registered routes.
      *
      * Each `Route` in this vector represents a distinct endpoint that can handle specific HTTP requests.
      */
      std::vector<Route> routes_;

      /**
      * @brief Maps HTTP error codes to their corresponding handler functions.
      *
      * This `unordered_map` associates specific HTTP error codes (e.g., "404", "500") with their respective
      * handler functions. Each handler function adheres to the `FunctionType` signature, accepting references
      * to a `Request` and a `Response` object and returning an `Awaitable<void>` for asynchronous processing.
      *
      * @details
      * When an HTTP error occurs during parsing, the server utilizes this map to locate and invoke the appropriate handler
      * function to generate a tailored error response. This mechanism allows for customizable and dynamic
      * error handling or logging based on the specific type of error encountered.
      *
      * **Key:**
      * - `std::string`: Represents the HTTP error code (e.g., "404" for Not Found, "500" for Internal Server Error). 
      * "Log" is a special key for logging, and will be called, if set, for all errors before checking for specific error handlers.
      *
      * **Value:**
      * - `FunctionType`: A handler function that processes the error by modifying the `Response` object accordingly.
      *
      * @note
      * - Ensure that all necessary error handlers are registered during the server initialization phase. otherwise, the server may return default error response.
      * - Handlers should be asynchronous to align with the `Awaitable<void>` return type, facilitating non-blocking operations.
      *
      * @see FunctionType
      * @see Route
      * @see HTTPEndpointHandler::match()
      * 
      * @todo not implemented yet
      */
      std::unordered_map<std::string, std::function<FunctionType>> error_page_handlers_;


      /**
      * @brief Unordered map of plain string routes for quick lookup.
      *
      * The key is the route's path pattern as a string, and the value is the corresponding `Route`.
      */
      std::unordered_map<std::string, Route> plainstring_routes_;

      /**
      * @brief Middleware chain executed for every request during the HEADER phase.
      *
      * Middleware functions added to this chain are executed before any route-specific middleware.
      * This is useful for global middleware such as logging or authentication.
      */
      MiddlewareChain middleware_chain_;///< Middleware executed for every request on headers

      /**
      * @brief Parses the path pattern into a regular expression and extracts parameter names.
      *
      * @param pathPattern The path pattern string (e.g., "/users/{id}").
      * @param outRegex Output parameter to store the compiled regular expression.
      * @param outParamNames Output parameter to store the extracted parameter names.
      *
      * @details Converts path patterns with parameters enclosed in `{}` into regex patterns.
      * Extracts the names of these parameters for later use in request handling.
      */
      void parsePathPattern(const std::string &pathPattern, std::regex &outRegex, std::vector<std::string> &outParamNames) const;
      
      size_t findMatchingBrace(const std::string &pathPattern, size_t start) const;

      bool containsCapturingGroup(const std::string &regex) const;

   public:
      /**
      * @brief Default constructor.
      *
      * Initializes an `HTTPEndpointHandler` with no routes or middleware.
      */
      HTTPEndpointHandler() = default;

      /**
      * @brief Default destructor.
      *
      * Cleans up any resources used by the `HTTPEndpointHandler`.
      */
      ~HTTPEndpointHandler() = default;

      /**
      * @brief Adds a plain string handler for a specific route.
      *
      * This function registers a route that matches an exact path string without regex.
      * It is optimized for routes that do not require pattern matching.
      *
      * @param method Set of allowed HTTP methods (e.g., GET, POST) for this route.
      * @param pathPattern The exact path string to match (e.g., "/home").
      * @param function The handler function to process matched requests.
      * @return Route& Reference to the newly added `Route` object.
      *
      * @see Route
      * @see FunctionType
      */
      Route &addPlainStringHandler(const std::set<std::string> &method, const std::string &pathPattern, std::function<FunctionType> function);

      /**
      * @brief Adds a handler for a route with a regex path pattern.
      *
      * This function registers a route that matches paths based on the provided regex pattern.
      * It is suitable for routes that include dynamic parameters.
      *
      * @param method Set of allowed HTTP methods (e.g., GET, POST) for this route.
      * @param pathPattern The regex-based path pattern to match (e.g., "/users/\\d+").
      * @param function The handler function to process matched requests.
      * @return Route& Reference to the newly added `Route` object.
      *
      * @see Route
      * @see FunctionType
      */
      Route &addHandler(const std::set<std::string> &method, const std::string &pathPattern, std::function<FunctionType> function);

      /**
      * @brief Adds a handler for a route with a string view method and regex path pattern.
      *
      * This function is similar to `addHandler` but accepts a `std::string_view` for the HTTP method.
      *
      * @param method A string view representing the HTTP method (e.g., "GET", "POST").
      * @param pathPattern The regex-based path pattern to match.
      * @param function The handler function to process matched requests.
      * @return Route& Reference to the newly added `Route` object.
      *
      * @see Route
      * @see FunctionType
      */
      Route &addHandler(std::string_view &method, const std::string &pathPattern, std::function<FunctionType> function);

      /**
      * @brief Adds a middleware function to a specified phase for all routes.
      *
      * Middleware functions added using this method are executed for every incoming request
      * during the specified phase. This is useful for global middleware such as authentication,
      * logging, or error handling.
      *
      * @param phase The phase during which the middleware should be executed.
      * @param middleware The middleware function to add.
      * @return MiddlewareChain& Reference to the global `MiddlewareChain` for chaining.
      *
      * @see MiddlewarePhase
      * @see MiddlewareFunctionType
      */
      MiddlewareChain &addMiddleware(MiddlewarePhase phase, std::function<MiddlewareFunctionType> middleware);

      /**
      * @brief Retrieves the global middleware chain.
      *
      * @return MiddlewareChain& Reference to the global `MiddlewareChain`.
      *
      * @see MiddlewareChain
      */
      MiddlewareChain &getMiddlewareChain();

      /**
      * @brief Matches an incoming request to a registered route.
      *
      * This function iterates through all registered routes and attempts to match the request
      * based on the HTTP method and path. If a matching route is found, it returns a pointer
      * to the `Route` and a boolean indicating whether the route was accepted as a plain string route.
      *
      * @param request The HTTP request to match.
      * @return std::optional<std::pair<Route*, bool>> An optional pair containing a pointer to the matched `Route`
      * and a boolean indicating if the route was found and method is allowed for that route (`true`) or if method is disallowed by this route (`false`).
      * Only for plain string routes, the boolean is always `true` for regex routes 
      * (for optimization, since checking each regex is expensive, and only then checking the method is expensive).
      * 
      * If no route matches, returns `std::nullopt`.
      *
      * @see Route
      */
      std::optional<std::pair<Route *, bool>> match(Request &request);

      void addErrorHandler(const std::string &error_code, std::function<FunctionType> function);
      void executeErrorChain(Request &request, Response &response);

   };

}// namespace usub::server::protocols::http

#endif// ENDPOINT_HANDLER_H
