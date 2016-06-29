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
 * Module to handle education of human resource management
 * 
 * Handling education data to handle management of human resources
 */

/**
 * Custom form to manage formal education of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_education_formal (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['datatitle'] = 'Educations';
  $form_state['storage']['context'] = 'formals';
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['getnewref'] = array(
    'path' => 'dnchrm-education/nojs/form/setelevel',
    'title' => 'Educational Level',
    'size' => 'medium',
  );
  $form_state['storage']['drupalrender'] = 'teaser';
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Produce popup form to add new educational level
 * 
 * @param string $js
 */
function dnchrm_education_add_new_elevel_popup($js = NULL) {
  // degrade half-assed to no js
  if(!$js) {
    return drupal_get_form('dnchrm_register_reference');
  }
  
  // adelante!
  ctools_include('modal');
  ctools_include('ajax');
  $form_state['title'] = t('Register Educational Level');
  $form_state['ajax'] = TRUE;
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['context'] = 'educational_levels';
  $output = ctools_modal_form_wrapper('dnchrm_register_reference', $form_state);
  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss(t('Success!'));
    $output[] = ctools_ajax_command_reload();
  }
  print ajax_render($output);
}

/**
 * Custom form to add formal education
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_education_formal_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'formals';
  $form_state['storage']['module'] = 'dnchrm_education';
  $form_state['storage']['destination'] = 'dnchrm/education/' . $form_state['storage']['person'] . '/add-title';
  return dnchrm_build_add_item_form($form, $form_state);
}

