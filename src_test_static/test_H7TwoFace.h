/*
 * test_H7TwoFace.h
 *
 *  Created on: 29.08.2024
 *      Author: martin.oberzalek
 */
#pragma once

#include <TestUtils.h>
#include <memory>

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write1();
std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write2();
std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write3();
std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write4();
std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write5();

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_delete1();

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_rename1();

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_max_files1();
std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_max_files2();

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_list_files1();

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_open_speacial1();

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write_ini1();
