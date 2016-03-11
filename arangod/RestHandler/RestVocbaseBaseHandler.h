////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_REST_HANDLER_REST_VOCBASE_BASE_HANDLER_H
#define ARANGOD_REST_HANDLER_REST_VOCBASE_BASE_HANDLER_H 1

#include "Basics/Common.h"
#include "Rest/HttpResponse.h"
#include "RestHandler/RestBaseHandler.h"
#include "RestServer/VocbaseContext.h"
#include "Utils/OperationResult.h"
#include "VocBase/vocbase.h"

struct TRI_vocbase_t;

namespace arangodb {

class VocbaseContext;

////////////////////////////////////////////////////////////////////////////////
/// @brief abstract base request handler
////////////////////////////////////////////////////////////////////////////////

class RestVocbaseBaseHandler : public RestBaseHandler {
 private:
  RestVocbaseBaseHandler(RestVocbaseBaseHandler const&) = delete;
  RestVocbaseBaseHandler& operator=(RestVocbaseBaseHandler const&) = delete;

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief batch path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const BATCH_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief cursor path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const CURSOR_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief document path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const DOCUMENT_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief edge path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const EDGE_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief edges path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const EDGES_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief document export path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const EXPORT_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief document import path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const IMPORT_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief replication path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const REPLICATION_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief simple query all path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const SIMPLE_QUERY_ALL_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief simple batch document lookup path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const SIMPLE_LOOKUP_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief simple batch document removal path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const SIMPLE_REMOVE_PATH;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief upload path
  //////////////////////////////////////////////////////////////////////////////

  static std::string const UPLOAD_PATH;

 public:
  explicit RestVocbaseBaseHandler(rest::HttpRequest*);

  ~RestVocbaseBaseHandler();

 protected:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief assemble a document id from a string and a string
  /// optionally url-encodes
  //////////////////////////////////////////////////////////////////////////////

  std::string assembleDocumentId(std::string const&, std::string const&, bool);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief check if a collection needs to be created on the fly
  ///
  /// this method will check the "createCollection" attribute of the request. if
  /// it is set to true, it will verify that the named collection actually
  /// exists.
  /// if the collection does not yet exist, it will create it on the fly.
  /// if the "createCollection" attribute is not set or set to false, nothing
  /// will
  /// happen, and the collection name will not be checked
  //////////////////////////////////////////////////////////////////////////////

  bool checkCreateCollection(std::string const&, TRI_col_type_e);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates a HTTP 201 or 202 response
  //////////////////////////////////////////////////////////////////////////////

  void generate20x(arangodb::OperationResult const&, std::string const&,
                   TRI_col_type_e);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates ok message without content
  //////////////////////////////////////////////////////////////////////////////

  void generateOk() { createResponse(rest::HttpResponse::NO_CONTENT); }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates ok message with no body but with certain status code
  //////////////////////////////////////////////////////////////////////////////

  void generateOk(rest::HttpResponse::HttpResponseCode code) {
    createResponse(code);
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates message for a saved document
  //////////////////////////////////////////////////////////////////////////////

  void generateSaved(arangodb::OperationResult const& result,
                     std::string const& collectionName, TRI_col_type_e type);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates deleted message
  //////////////////////////////////////////////////////////////////////////////

  void generateDeleted(arangodb::OperationResult const& result,
                       std::string const& collectionName, TRI_col_type_e type);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates document not found error message, no transaction info
  //////////////////////////////////////////////////////////////////////////////

  void generateDocumentNotFound(std::string const& /* collection name */,
                                std::string const& /* document key */) {
    generateError(rest::HttpResponse::NOT_FOUND, TRI_ERROR_ARANGO_DOCUMENT_NOT_FOUND);
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates not implemented
  //////////////////////////////////////////////////////////////////////////////

  void generateNotImplemented(std::string const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates forbidden
  //////////////////////////////////////////////////////////////////////////////

  void generateForbidden();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates precondition failed, without transaction info
  ///        DEPRECATED
  //////////////////////////////////////////////////////////////////////////////

  void generatePreconditionFailed(std::string const&, std::string const& key,
                                  TRI_voc_rid_t rev);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates precondition failed, without transaction info
  //////////////////////////////////////////////////////////////////////////////

  void generatePreconditionFailed(arangodb::velocypack::Slice const& slice);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates not modified
  //////////////////////////////////////////////////////////////////////////////

  void generateNotModified(TRI_voc_rid_t);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generates first entry from a result set
  //////////////////////////////////////////////////////////////////////////////

  void generateDocument(arangodb::velocypack::Slice const&, bool,
                        arangodb::velocypack::Options const* options = nullptr);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generate an error message for a transaction error
  ///        DEPRECATED
  //////////////////////////////////////////////////////////////////////////////

  void generateTransactionError(std::string const&, int, std::string const& key,
                                TRI_voc_rid_t = 0);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief generate an error message for a transaction error
  //////////////////////////////////////////////////////////////////////////////
  
  void generateTransactionError(OperationResult const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief extracts the revision
  ///
  /// @note @FA{header} must be lowercase.
  //////////////////////////////////////////////////////////////////////////////

  TRI_voc_rid_t extractRevision(char const*, char const*, bool&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief extracts a boolean parameter value
  //////////////////////////////////////////////////////////////////////////////

  bool extractBooleanParameter(char const* name, bool def) const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief parses the body as VelocyPack
  //////////////////////////////////////////////////////////////////////////////

  std::shared_ptr<VPackBuilder> parseVelocyPackBody(VPackOptions const*, bool&);

 protected:

  //////////////////////////////////////////////////////////////////////////////
  /// @brief request context
  //////////////////////////////////////////////////////////////////////////////

  VocbaseContext* _context;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief the vocbase
  //////////////////////////////////////////////////////////////////////////////

  TRI_vocbase_t* _vocbase;

 public:
  bool isDirect() const override { return false; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief prepareExecute, to react to X-Arango-Nolock header
  //////////////////////////////////////////////////////////////////////////////

  virtual void prepareExecute() override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief finalizeExecute, to react to X-Arango-Nolock header
  //////////////////////////////////////////////////////////////////////////////

  virtual void finalizeExecute() override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief _nolockHeaderFound
  //////////////////////////////////////////////////////////////////////////////

  bool _nolockHeaderFound;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief _nolockHeaderFound
  //////////////////////////////////////////////////////////////////////////////

  std::unordered_set<std::string>* _nolockHeaderSet;
};
}

#endif
