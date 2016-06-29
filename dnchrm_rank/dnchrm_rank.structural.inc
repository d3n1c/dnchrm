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
 * Module to handle structural rank of human resource management
 * 
 * Handling structural rank data to handle management of human resources
 */

/**
 * Custom form to manage structural rank of given person
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   nid of the person
 * 
 * @return array
 *   form array
 */
function dnchrm_rank_structural (array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  $form_state['storage']['datatitle'] = 'Ranks';
  $form_state['storage']['context'] = 'structural_ranks';
  $form_state['storage']['module'] = 'dnchrm_rank';
  $form_state['storage']['drupalrender'] = 'teaser';
  
  dnchrm_build_tabled_infos_form($form, $form_state);
  if (!empty($form['ctable']['#rows'])) {
    $sorting = array();
    $dump = array();
    foreach ($form['ctable']['#rows'] as $keys => $values) {
      $nid = explode('-', $values[0]['data']['#id']);
      $nid = trim($nid[1]);
      settype($nid, 'int');
      $data = dncsystem_content_get_data('dnchrm_rank', 'structural_ranks', $nid);
      if (!empty($data[$nid])) {
        $data = $data[$nid];
        $tid = $data['type_of_rank'];
        $tdata = dncsystem_content_get_data('dnchrm_rank', 'types_of_rank', $tid);
        if (!empty($tdata[$tid])) {
          $tdata = $tdata[$tid];
          $sorting[$nid] = $tdata['level'];
          $dump[$nid] = $values;
        }
        unset($tid, $tdata);
      }
      unset ($nid, $data);
    }
    $form['ctable']['#rows'] = array();
    arsort($sorting);
    $i = 0;
    foreach ($sorting as $keys => $values) {
      $i++;
      $dump[$keys][0]['data']['#markup'] = $i . '.';
      $form['ctable']['#rows'][] = $dump[$keys];
    }
    unset ($sorting, $dump, $i);
  }
  return $form;
}

/**
 * Produce popup form to add new educational level
 * 
 * @param string $js
 */
function dnchrm_rank_add_new_trank_popup($js = NULL, $structural = TRUE) {
  // degrade half-assed to no js
  $date = dnchrm_rank_get_type_of_rank_max_date($structural);
  if(!$js) {
//    return drupal_get_form('dnchrm_register_reference');
    $filter = array(
      'effective_date' => $date,
    );
    if (!empty($structural)) {
      $filter['structural_boolean'] = $structural;
    }
    $filter = urlencode(base64_encode(serialize($filter)));
    return drupal_get_form('dnchrm_rank_types_of_rank_references', $filter);
    unset ($filter);
  }
  
  // adelante!
  ctools_include('modal');
  ctools_include('ajax');
  $form_state['title'] = t('Register Type of Rank');
  $form_state['ajax'] = TRUE;
  $form_state['storage']['effective_date'] = $date;
  $form_state['storage']['structural_boolean'] = empty($structural) ? FALSE : TRUE;
  $output = ctools_modal_form_wrapper('dnchrm_rank_types_of_rank_references_form', $form_state);
  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss(t('Success!'));
    $output[] = ctools_ajax_command_reload();
  }
  print ajax_render($output);
}

/**
 * Custom form to add structural rank
 * 
 * @param array $form
 * @param array $form_state
 * @param int $person
 *   NID of the person
 * 
 */
function dnchrm_rank_structural_add(array $form = array(), array $form_state = array(), $person = NULL) {
  if (empty($form_state['storage']['person'])) {
    if (empty($person)) {
      return $form;
    }
    $form_state['storage']['person'] = $person;
  }
  
  $form_state['storage']['context'] = 'structural_ranks';
  $form_state['storage']['module'] = 'dnchrm_rank';
  $form_state['storage']['destination'] = 'dnchrm/rank/' . $form_state['storage']['person'] . '/structurals';
  return dnchrm_build_add_item_form($form, $form_state);
}

