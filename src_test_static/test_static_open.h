/*
 * test_static_open.h
 *
 *  Created on: 26.08.2024
 *      Author: martin.oberzalek
 */

#pragma once

#include <TestUtils.h>
#include <memory>

std::shared_ptr<TestCaseBase<bool>> test_case_static_open1();
std::shared_ptr<TestCaseBase<bool>> test_case_static_open2();
std::shared_ptr<TestCaseBase<bool>> test_case_static_open3();
std::shared_ptr<TestCaseBase<bool>> test_case_static_open4();
std::shared_ptr<TestCaseBase<bool>> test_case_static_open5();
std::shared_ptr<TestCaseBase<bool>> test_case_static_open6();

std::shared_ptr<TestCaseBase<bool>> test_case_static_write1();
std::shared_ptr<TestCaseBase<bool>> test_case_static_write2();
std::shared_ptr<TestCaseBase<bool>> test_case_static_write3();
std::shared_ptr<TestCaseBase<bool>> test_case_static_write4();

std::shared_ptr<TestCaseBase<bool>> test_case_static_read1();
std::shared_ptr<TestCaseBase<bool>> test_case_static_read2();
