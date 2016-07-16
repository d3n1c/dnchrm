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
 * Module to handle organiational experiences of human resource management
 * 
 * Handling organizational experiences data to handle management of human resources
 */

/**
 * Custom form to manage organizational experiences of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_experience_organizational (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['datatitle'] = 'Organizational Experiences';
  $form_state['storage']['context'] = 'organizational_experiences';
  $form_state['storage']['module'] = 'dnchrm_experience';
  $form_state['storage']['wrapfields'] = TRUE;
  
  return dnchrm_build_tabled_infos_form($form, $form_state);
}

/**
 * Custom form to add organizational experiences
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_experience_organizational_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'organizational_experiences';
  $form_state['storage']['module'] = 'dnchrm_experience';
  $form_state['storage']['destination'] = 'dnchrm/experience/' . $form_state['storage']['person'] . '/organizationals';
  return dnchrm_build_add_item_form($form, $form_state);
}

