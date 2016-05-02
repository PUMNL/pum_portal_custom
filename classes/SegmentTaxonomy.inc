<?php

/**
 * @author Jaap Jansma (CiviCooP) <jaap.jansma@civicoop.org>
 * @license http://www.gnu.org/licenses/agpl-3.0.html
 */
class SegmentTaxonomy
{

    public static function pre($op, $objectName, $id, &$params) {
        self::unlinkTaxonomyTerm($op, $objectName, $id, $params);
    }

    public static function post($op, $objectName, $id, &$objectRef) {
        self::updateTaxonomyTermName($op, $objectName, $id, $objectRef);
        self::setSectorCoordinator($op, $objectName, $id, $objectRef);
    }

    /**
     * Unlink taxonomy when a segment is deleted
     *
     * @param $op
     * @param $objectName
     * @param $id
     * @param $params
     */
    protected static function unlinkTaxonomyTerm($op, $objectName, $id, $params) {
        if ($objectName != 'Segment' && $op != 'delete') {
            return;
        }

        $terms = self::getTermsBySegmentId($id, false);
        foreach($terms as $term) {
            $term->field_pum_segment_id["und"][0]["value"] = '';
            taxonomy_term_save($term);
        }
    }

    /**
     * Set the sector coordinator of the term
     *
     * @param $op
     * @param $objectName
     * @param $id
     * @param $objectRef
     */
    protected static function setSectorCoordinator($op, $objectName, $id, &$objectRef) {
        if ($objectName != 'ContactSegment' && ($op != 'create' || $op !='edit')) {
            return;
        }

        $civicoop_segment_role_option_group = civicrm_api3('OptionGroup', 'getvalue', array('name' => 'civicoop_segment_role', 'return' => 'id'));
        $sc_role = civicrm_api3('OptionValue', 'getvalue', array('option_group_id' => $civicoop_segment_role_option_group, 'name' => 'sector_coordinator', 'return' => 'value'));

        if ($objectRef->role_value != $sc_role) {
            return;
        }

        $terms = self::getTermsBySegmentId($objectRef->segment_id, false);
        $sector_coorinator_id = self::getSectorCoordinatorId($objectRef->segment_id);
        foreach($terms as $term) {
            $term->field_pum_coordinator_id["und"][0]["value"]  = $sector_coorinator_id ? $sector_coorinator_id : null;
            taxonomy_term_save($term);
        }
    }

    /**
     * Updates the name of a taxonomy term when the name of the segment is changed
     * If segment does not exist adds a taxonomy term
     *
     * @param $op
     * @param $objectName
     * @param $id
     * @param $objectRef
     */
    protected static function updateTaxonomyTermName($op, $objectName, $id, &$objectRef) {
        if ($objectName != 'Segment' || ($op != 'create' && $op != 'edit')) {
            return;
        }

        //do not continue when migration from sector tags to contact segment is running
        if (function_exists('_contact_segment_migrate_is_running')) {
            $migrateRunning = _contact_segment_migrate_is_running();
            if ($migrateRunning) {
                return;
            }
        }

        if ($objectRef->parent_id != null) {
            //do not update areas of expertise
            return;
        }

        $terms = self::getTermsBySegmentId($id, true);
        $sector_coorinator_id = self::getSectorCoordinatorId($id);
        foreach($terms as $term) {
            $term->name = $objectRef->label;
            $term->field_pum_coordinator_id["und"][0]["value"]  = $sector_coorinator_id ? $sector_coorinator_id : null;
            taxonomy_term_save($term);
        }
    }

    protected static function getSectorCoordinatorId($segment_id) {
        $civicoop_segment_role_option_group = civicrm_api3('OptionGroup', 'getvalue', array('name' => 'civicoop_segment_role', 'return' => 'id'));
        $role = civicrm_api3('OptionValue', 'getvalue', array('option_group_id' => $civicoop_segment_role_option_group, 'name' => 'sector_coordinator', 'return' => 'value'));
        try {
            $contact_segment = civicrm_api3('ContactSegment', 'getsingle', array('segment_id' => $segment_id, 'is_active' => 1, 'role_value' => $role));
            return $contact_segment['contact_id'];
        } catch (Exception $e) {
            //do nothing
        }
        return false;
    }

    protected static function getTermsBySegmentId($segment_id, $create_new_one=true) {
        return SegmentTerms::getTermsBySegmentId($segment_id, $create_new_one);
    }

}