<?php

/**
 * @author Jaap Jansma (CiviCooP) <jaap.jansma@civicoop.org>
 * @license http://www.gnu.org/licenses/agpl-3.0.html
 */
class SegmentSectorCommunity {

    private static $originalContactSegment = false;

    public static function pre($op, $objectName, $id, &$params) {
        if ($objectName != 'ContactSegment' && $op != 'create') {
            return;
        }

        self::$originalContactSegment = civicrm_api3('ContactSegment', 'getsingle', array('id' => $id));
    }

    public static function post($op, $objectName, $id, &$objectRef) {
        if ($objectName != 'ContactSegment') {
            return;
        }

        $civicoop_segment_role_option_group = civicrm_api3('OptionGroup', 'getvalue', array('name' => 'civicoop_segment_role', 'return' => 'id'));
        $expert_role = civicrm_api3('OptionValue', 'getvalue', array('option_group_id' => $civicoop_segment_role_option_group, 'name' => 'expert', 'return' => 'value'));

        if ($objectRef->role_value != $expert_role) {
            //only process for expert roles
            return;
        }

        $segment = civicrm_api3('Segment', 'getsingle', array('id' => $objectRef->segment_id));
        if (!empty($segment['parent_id'])) {
            //do not link area of expertise
            return;
        }

        $action = 'none';
        if ($op == 'create' && $objectRef->is_active) {
            $action = 'subscribe';
        } elseif ($op == 'edit' && self::$originalContactSegment) {
            if (self::$originalContactSegment['is_active'] && !$objectRef->is_active) {
                $action = 'unsubscribe';
            } elseif (!self::$originalContactSegment['is_active'] && $objectRef->is_active) {
                $action = 'subscribe';
            }
        } elseif ($op == 'delete') {
            $action = 'unsubscribe';
        }

        if ($action == 'subscribe') {
            self::subscribe($objectRef->contact_id, $objectRef->segment_id);
        } elseif ($action == 'unsubscribe') {
            self::unsubscribe($objectRef->contact_id, $objectRef->segment_id);
        }
    }

    protected static function subscribe($contact_id, $segment_id) {
        $uid = self::user_id($contact_id);
        if (!$uid) {
            return;
        }
        $account = user_load($uid);
        $account_edit = array();
        $account_edit["field_pum_user_sector"] = $account->field_pum_user_sector;

        $terms = SegmentTerms::getTermsBySegmentId($segment_id, false);
        foreach($terms as $term) {
            $account_edit["field_pum_user_sector"]["und"][] = array('tid' => $term->tid);
        }
        user_save($account, $account_edit);
    }

    protected static function unsubscribe($contact_id, $segment_id) {
        $uid = self::user_id($contact_id);
        if (!$uid) {
            return;
        }
        $account = user_load($uid);
        $account_edit = array();
        $account_edit["field_pum_user_sector"] = $account->field_pum_user_sector;

        if (isset($account_edit["field_pum_user_sector"]['und']) && is_array($account_edit["field_pum_user_sector"]['und'])) {
            $terms = SegmentTerms::getTermsBySegmentId($segment_id, false);
            foreach ($account_edit["field_pum_user_sector"]['und'] as $v => $t) {
                foreach($terms as $term) {
                    if ($term->tid == $t['tid']) {
                        unset($account_edit["field_pum_user_sector"]['und'][$v]);
                    }
                }
            }
        }
        user_save($account, $account_edit);
    }

    protected static function user_id($contact_id) {
        try {
            return civicrm_api3('UFMatch', 'getvalue', array(
                'contact_id' => $contact_id,
                'return' => 'uf_id',
            ));
        } catch (Exception $e) {
            //do nothing
        }
        return false;
    }

}