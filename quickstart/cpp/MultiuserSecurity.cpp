/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * The MultiuserSecurityExample QuickStart Example.
 *
 * This example takes the following steps:
 *
 * 1. Create a Geode Cache with multiuser enabled.
 * 2. Creates userCache using user "root". Who is authorized to do get and put
 * operations.
 * 3. Creates userCache using user "writer2". Who is authorized to do only put
 * operation. It tries to do get operation and gets NotAuthorizedException.
 * 4.  Close the Cache.
 *
 */

// Include the Geode library.
#include <geode/GeodeCppCache.hpp>

// Use the "geode" namespace.
using namespace apache::geode::client;

char* getFuncIName = (char*)"MultiGetFunctionI";

void runWithUserRoot(CachePtr cachePtr) {
  // user root's credential who is authorized to do put/get operations
  PropertiesPtr credentials = Properties::create();

  credentials->insert("security-username", "root");
  credentials->insert("security-password", "root");

  // Create user cache by passing credentials
  RegionServicePtr userCache1 = cachePtr->createAuthenticatedView(credentials);

  // Create region using usercache
  RegionPtr regionPtr1 = userCache1->getRegion("partition_region");

  LOGINFO("Obtained the Region from the Cache");

  CacheableKeyPtr key = CacheableKey::create("Key1");

  // doing operation on behalf of user "writer2"
  regionPtr1->put(key, "Value1");

  LOGINFO("Entry created in the Region");

  // Get Entries back out of the Region.
  CacheablePtr result1Ptr = regionPtr1->get(key);

  // to execute function on server
  bool getResult = true;
  CacheableVectorPtr routingObj = CacheableVector::create();

  routingObj->push_back(key);

  // test data independant function with result on one server
  LOGINFO("test data independant function with result on one server");
  CacheablePtr args = routingObj;
  char buf[128];

  ExecutionPtr exc = FunctionService::onServer(userCache1);

  CacheableVectorPtr executeFunctionResult =
      exc->withArgs(args)->execute(getFuncIName, getResult)->getResult();
  CacheableVectorPtr resultList = CacheableVector::create();
  if (executeFunctionResult == nullptr) {
    LOGINFO("get executeFunctionResult is NULL");
  } else {
    for (int item = 0; item < executeFunctionResult->size(); item++) {
      auto arrayList = std::dynamic_pointer_cast<CacheableArrayList>(
          executeFunctionResult->operator[](item));
      for (int pos = 0; pos < arrayList->size(); pos++) {
        resultList->push_back(arrayList->operator[](pos));
      }
    }
    sprintf(buf, "get: result count = %d", resultList->size());
    LOGINFO(buf);

    for (int i = 0; i < resultList->size(); i++) {
      sprintf(buf, "get result[%d]=%s", i,
              std::dynamic_pointer_cast<CacheableString>(resultList->operator[](i))->asChar());
      LOGINFO(buf);
    }
  }
  // test for Query

  // Execute a Query which returns a ResultSet.
  QueryServicePtr qrySvcPtr = userCache1->getQueryService();
  QueryPtr qryPtr =
      qrySvcPtr->newQuery("SELECT DISTINCT * FROM /partition_region");
  SelectResultsPtr resultsPtr = qryPtr->execute();

  LOGINFO("ResultSet Query returned %d rows", resultsPtr->size());

  // close the user cache
  userCache1->close();

  LOGINFO("User root done put/get ops successfully");
}

void runWithUserWriter(CachePtr cachePtr) {
  // user writer2's credentials who is authorixed to do only put operation
  PropertiesPtr credentials = Properties::create();

  credentials->insert("security-username", "writer2");
  credentials->insert("security-password", "writer2");

  // Create user cache by passing credentials
  RegionServicePtr userCache2 = cachePtr->createAuthenticatedView(credentials);

  // Create region using usercache
  RegionPtr regionPtr2 = userCache2->getRegion("partition_region");

  LOGINFO("Entry created in the Region");

  // Get Entries back out of the Region.
  CacheablePtr result1Ptr = regionPtr2->get("Key1");

  // collect NotAuthorized exception

  // close the user cache
  userCache2->close();
}

// The MultiuserSecurityExample QuickStart example.
int main(int argc, char** argv) {
  try {
    PropertiesPtr secProp = Properties::create();

    // By setting this property client will send credential in encrypted form.
    // to do this one need to setup OpenSSL.
    // secProp->insert("security-client-dhalgo", "Blowfish:128");

    // Connect to the Geode Distributed System using the settings from the
    // geode.properties file by default, programatically
    // overriding secProp properties.

    // Create a Geode Cache.
    CacheFactoryPtr cacheFactory = CacheFactory::createCacheFactory(secProp);

    CachePtr cachePtr =
        cacheFactory->setMultiuserAuthentication(true)->create();

    LOGINFO("Created the Geode Cache with multiuser enable.");

    RegionFactoryPtr regionFactory = cachePtr->createRegionFactory(PROXY);

    LOGINFO("Created the RegionFactory");

    // Create the example Region Programmatically.
    RegionPtr regionPtr = regionFactory->create("partition_region");

    runWithUserRoot(cachePtr);

    try {
      runWithUserWriter(cachePtr);
      LOGINFO(
          "Failed:  Didn't get expected authorization exception while user "
          "writer2 was doing get operation.");

      return 1;
    } catch (const apache::geode::client::NotAuthorizedException& expected) {
      LOGINFO(
          "Got expected authorization failure while obtaining the Entry: %s",
          expected.getMessage());
      LOGINFO(
          "Got expected authorization exception while user writer2 was doing "
          "get operation.");
    }

    // Close the Geode Cache.
    cachePtr->close();

    LOGINFO("Closed the Geode Cache");

    return 0;
  }
  // An exception should not occur
  catch (const Exception& geodeExcp) {
    LOGERROR("FAILED:MultiuserSecurityExample Geode Exception: %s",
             geodeExcp.getMessage());

    return 1;
  }
}
