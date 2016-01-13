<?php
/**
 * @author Jaap Jansma (CiviCooP) <jaap.jansma@civicoop.org>
 * @license http://www.gnu.org/licenses/agpl-3.0.html
 */

class SegmentTerms {

    /**
     * Returns the terms belonging to this segment
     *
     * @param $segment_id
     * @param bool $create_new_one
     * @return array
     */
    public static function getTermsBySegmentId($segment_id, $create_new_one=true) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'taxonomy_term')
            ->fieldCondition('field_pum_segment_id', 'value', $segment_id, '=');
        $result = $query->execute();
        if (isset($result['taxonomy_term']) && count($result['taxonomy_term'])) {
            $return = array();
            foreach($result['taxonomy_term'] as $tid => $term) {
                $return[] = taxonomy_term_load($tid);
            }
            return $return;
        } elseif ($create_new_one) {
            $sector_vocabulary = taxonomy_vocabulary_machine_name_load("sector");
            $term = new stdClass();
            $term->vid = $sector_vocabulary->vid;
            $term->field_pum_segment_id["und"][0]["value"] = $segment_id;

            return array($term);
        }
        return array();
    }
}