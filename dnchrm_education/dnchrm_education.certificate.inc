<?php

/* 
 * Copyright (C) 2016 denic wibowo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * Module to handle certificate of human resource management
 * 
 * Handling certificate data to handle management of human resources
 */

/**
 * Custom form to manage certificate of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_education_certificate (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['datatitle'] = 'Educations';
  $form_state['storage']['context'] = 'certificates';
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['drupalrender'] = 'teaser';
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Custom form to add certificate education
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_education_certificate_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'certificates';
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['destination'] = 'dnchrm/education/' . $form_state['storage']['person'] . '/certificates';
  return dnchrm_build_add_item_form($form, $form_state);
}

