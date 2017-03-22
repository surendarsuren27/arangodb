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
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#include "ClusterEdgeCursor.h"

#include "Cluster/ClusterMethods.h"
#include "Cluster/ClusterTraverser.h"
#include "Transaction/Helpers.h"
#include "Transaction/Methods.h"

#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

using ClusterEdgeCursor = arangodb::traverser::ClusterEdgeCursor;

ClusterEdgeCursor::ClusterEdgeCursor(StringRef vertexId, uint64_t depth,
                                     arangodb::traverser::ClusterTraverser* traverser)
    : _position(0), _resolver(traverser->_trx->resolver()) {
      transaction::BuilderLeaser leased(traverser->_trx);
      
#warning fix copying by either adding overloaded method or
      transaction::BuilderLeaser b(traverser->_trx);
      b->add(VPackValue(vertexId.toString()));
      
      fetchEdgesFromEngines(traverser->_dbname, traverser->_engines, b->slice(), depth,
                            traverser->_edges, _edgeList, traverser->_datalake,
                            *(leased.get()), traverser->_filteredPaths,
                            traverser->_readDocuments);
      
    }

bool ClusterEdgeCursor::next(std::function<void(std::string const&,
                                                VPackSlice, size_t)> callback) {
  if (_position < _edgeList.size()) {
    VPackSlice edge = _edgeList[_position];
    std::string eid = transaction::helpers::extractIdString(_resolver, edge, VPackSlice());
    callback(eid, edge, _position);
    ++_position;
    return true;
  }
  return false;
}

bool ClusterEdgeCursor::readAll(std::unordered_set<VPackSlice>& result, size_t& cursorId) {
  if (_position == 0) {
    // We have not yet returned anything. So we simply return everything at once.
    std::copy(_edgeList.begin(), _edgeList.end(), std::inserter(result, result.end()));
    _position++;
    return true;
  }
  // We have already returned everything last time.
  return false;
}
