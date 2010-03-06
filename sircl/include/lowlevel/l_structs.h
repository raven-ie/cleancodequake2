/* sircl
 * Copyright (C) 2004 Francisco Javier Y��ez Fern�ndez.       
 *                                                                      
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or    
 * (at your option) any later version.                                  
 *                                                                      
 * This program is distributed in the hope that it will be useful,      
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        
 * GNU General Public License for more details.                         
 *                                                                      
 * You should have received a copy of the GNU General Public License    
 * along with this program; if not, write to the Free Software          
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA                  
 *                                                                      
 */

#ifndef _l_structs_h
#define _l_structs_h


/*******************************************************************/
//  low-level structs.
/*******************************************************************/

struct _irc_message {
   
    char *raw;
    
    char *prefix;
    
    int  command;

    char *command_str;
    
    char *params;
    
    char **param;

    int number_of_params;

    /* FROM_SERVER, FROM_USER, NONE */
    int from;

    int trailing;
};

/*******************************************************************/


#endif

