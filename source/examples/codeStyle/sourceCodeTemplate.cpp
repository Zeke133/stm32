/**
 * @file    sourceCodeTemplate.cpp
 * @author  Denis Homutovski
 * @version V1.0.0
 * @date    12-12-2018
 * @brief   Class source template.
 * @details   Class header code-style template.
 * @pre       -
 * @bug       -
 * @warning   -
 * @copyright GNU Public License.
 */

#include <headerCodeTemplate.hpp>

/**@{*/
/** Grouped description. */
uint8_t ClassTempate::staticAtribute1 = 0;
uint8_t ClassTempate::staticAtribute2 = 0;
/**@}*/

/**
 * @brief  ClassTempate constructor.
 * @param  paramName: Description.
 */
ClassTempate::ClassTempate(/**/) {
    // : attr1(param1),
    //   attr2(param2) {

    someAttribute = (uint8_t *)&constant;

}

/**
 * @brief  Here must be a method description.
 *         bla-bla-bla...
 *         ...
 * @param  paramName: Description.
 * @retval None
 */
inline void ClassTempate::privateFunction(void) {
    
    // comments
    ++staticAtribute2;
}

/**
 * Some text.
 * About public method.
 */
void ClassTempate::publicFunction(void) const {

    ++staticAtribute1;
}
