// Copyright 2023 Huawei Cloud Computing Technology Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/buildtool/execution_api/execution_service/ac_server.hpp"

#include "fmt/format.h"
#include "src/buildtool/compatibility/native_support.hpp"
#include "src/buildtool/execution_api/local/garbage_collector.hpp"

auto ActionCacheServiceImpl::GetActionResult(
    ::grpc::ServerContext* /*context*/,
    const ::build::bazel::remote::execution::v2::GetActionResultRequest*
        request,
    ::build::bazel::remote::execution::v2::ActionResult* response)
    -> ::grpc::Status {
    logger_.Emit(LogLevel::Trace,
                 "GetActionResult: {}",
                 NativeSupport::Unprefix(request->action_digest().hash()));
    auto lock = GarbageCollector::SharedLock();
    if (!lock) {
        auto str = fmt::format("Could not acquire SharedLock");
        logger_.Emit(LogLevel::Error, str);
        return grpc::Status{grpc::StatusCode::INTERNAL, str};
    }
    auto x = ac_.CachedResult(request->action_digest());
    if (!x) {
        return grpc::Status{grpc::StatusCode::NOT_FOUND,
                            fmt::format("{} missing from AC",
                                        NativeSupport::Unprefix(
                                            request->action_digest().hash()))};
    }
    *response = *x;
    return ::grpc::Status::OK;
}

auto ActionCacheServiceImpl::UpdateActionResult(
    ::grpc::ServerContext* /*context*/,
    const ::build::bazel::remote::execution::v2::UpdateActionResultRequest*
    /*request*/,
    ::build::bazel::remote::execution::v2::ActionResult* /*response*/)
    -> ::grpc::Status {
    auto const* str = "UpdateActionResult not implemented";
    logger_.Emit(LogLevel::Error, str);
    return ::grpc::Status{grpc::StatusCode::UNIMPLEMENTED, str};
}
