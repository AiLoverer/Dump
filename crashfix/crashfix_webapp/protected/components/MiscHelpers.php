<?php

class MiscHelpers
{	
	/**
	 * 	This helper function converts a file size number to human-readable string.
	 */
	public static function fileSizeToStr($fileSize, $fracLength = 1)
	{	
		// Convert file size to string, e.g. 100 KB, 100 MB etc.
		if ($fileSize < 1024) 
		{
			return round($fileSize, 0) .' byte(s)';
		} 
		elseif ($fileSize < 1048576) 
		{
			return round($fileSize / 1024, 0) .' KB';
		} 
		elseif ($fileSize < 1073741824) 
		{
			return round($fileSize / 1048576, $fracLength) . ' MB';
		} 
		else
		{
			return round($fileSize / 1073741824, $fracLength) . ' GB';
		} 		
	}
	
	/**
	 * Truncates a long string and adds an ellipsis (...) to the end of string.
	 * @param type $string
	 * @param type $length
	 * @param type $end
	 * @return type 
	 */
	public static function addEllipsis($string, $length, $end='...')
	{
		if (strlen($string) > $length)
		{
			$length -=  strlen($end);  // $length =  $length – strlen($end);
			$string  = substr($string, 0, $length);
			$string .= $end;  //  $string =  $string . $end;
		}
		
		return $string;
	}
	
	/**
	 * Splits a string containing space-separated numbers into an array.
	 * @param string $listStr String containing numbers.
	 * @return array On success, returns list of number; on failure, returns false.
	 */
	public static function splitNumberList($listStr)
	{
		// Convert string to array
		$numbers = preg_split('/[\s,]+/', $listStr, 0, PREG_SPLIT_NO_EMPTY);
		
		// Remove duplicates
		$numbers = array_unique($numbers);
		
		// Resulting array
		$result = array();
		
		foreach($numbers as $number)
		{	
			if(trim($number)=="")
				continue;
			
			if(!is_numeric($number))
			{					
				return false;
			}
			
			// Add to resulting array.
			$result[] = (int)$number;
		}
		
		// Return resulting array
		return $result;
	}
    
    /**
     * The phunction PHP framework (http://sourceforge.net/projects/phunction/) 
     * uses the following function to generate valid version 4 UUIDs:
     * @return string GUID 
     */
    public static function GUID()
    {
        if (function_exists('com_create_guid') === true)
        {
            return trim(com_create_guid(), '{}');
        }

        return sprintf('%04X%04X-%04X-%04X-%04X-%04X%04X%04X', 
                mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535), 
                mt_rand(16384, 20479), mt_rand(32768, 49151), 
                mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535));
    }
	
    /**
     * Converts a two-letter country code to a country name (e.g. US=>United States).
     * @param string $code Two-letter country code.
     * @return string Country name.
     */
	public static function countryCodeToCountryName($code)
	{
		// Convert to upper case
		$code = strtoupper($code);
		
		$map = array(
			'AF'=>'Afghanistan',
			'AX'=>'Еland Islands',
			'AL'=>'Albania',
			'DZ'=>'Algeria',
			'AS'=>'American Samoa',
			'AD'=>'Andorra',
			'AO'=>'Angola',
			'AI'=>'Anguilla',
			'AQ'=>'Antarctica',
			'AG'=>'Antigua and Barbuda',
			'AR'=>'Argentina',
			'AM'=>'Armenia',
			'AW'=>'Aruba',
			'AU'=>'Australia',
			'AT'=>'Austria',
			'AZ'=>'Azerbaijan',
			'BS'=>'Bahamas',
			'BH'=>'Bahrain',
			'BD'=>'Bangladesh',
			'BB'=>'Barbados',
			'BY'=>'Belarus',
			'BE'=>'Belgium',
			'BZ'=>'Belize',
			'BJ'=>'Benin',
			'BM'=>'Bermuda',
			'BT'=>'Bhutan',
			'BO'=>'Bolivia',
			'BA'=>'Bosnia and Herzegovina',
			'BW'=>'Botswana',
			'BV'=>'Bouvet Island',
			'BR'=>'Brazil',
			'IO'=>'British Indian Ocean Territory',
			'BN'=>'Brunei Darussalam',
			'BG'=>'Bulgaria',
			'BF'=>'Burkina Faso',
			'BI'=>'Burundi',
			'KH'=>'Cambodia',
			'CM'=>'Cameroon',
			'CA'=>'Canada',
			'CV'=>'Cape Verde',
			'KY'=>'Cayman Islands',
			'CF'=>'Central African Republic',
			'TD'=>'Chad',
			'CL'=>'Chile',
			'CN'=>'China',
			'CX'=>'Christmas Island',
			'CC'=>'Cocos (Keeling) Islands',
			'CO'=>'Colombia',
			'KM'=>'Comoros',
			'CG'=>'Congo',
			'CD'=>'Congo, The Democratic Republic of The',
			'CK'=>'Cook Islands',
			'CR'=>'Costa Rica',
			'CI'=>'Cote D\'ivoire',
			'HR'=>'Croatia',
			'CU'=>'Cuba',
			'CY'=>'Cyprus',
			'CZ'=>'Czech Republic',
			'DK'=>'Denmark',
			'DJ'=>'Djibouti',
			'DM'=>'Dominica',
			'DO'=>'Dominican Republic',
			'EC'=>'Ecuador',
			'EG'=>'Egypt',
			'SV'=>'El Salvador',
			'GQ'=>'Equatorial Guinea',
			'ER'=>'Eritrea',
			'EE'=>'Estonia',
			'ET'=>'Ethiopia',
			'FK'=>'Falkland Islands (Malvinas)',
			'FO'=>'Faroe Islands',
			'FJ'=>'Fiji',
			'FI'=>'Finland',
			'FR'=>'France',
			'GF'=>'French Guiana',
			'PF'=>'French Polynesia',
			'TF'=>'French Southern Territories',
			'GA'=>'Gabon',
			'GM'=>'Gambia',
			'GE'=>'Georgia',
			'DE'=>'Germany',
			'GH'=>'Ghana',
			'GI'=>'Gibraltar',
			'GR'=>'Greece',
			'GL'=>'Greenland',
			'GD'=>'Grenada',
			'GP'=>'Guadeloupe',
			'GU'=>'Guam',
			'GT'=>'Guatemala',
			'GG'=>'Guernsey',
			'GN'=>'Guinea',
			'GW'=>'Guinea-bissau',
			'GY'=>'Guyana',
			'HT'=>'Haiti',
			'HM'=>'Heard Island and Mcdonald Islands',
			'VA'=>'Holy See (Vatican City State)',
			'HN'=>'Honduras',
			'HK'=>'Hong Kong',
			'HU'=>'Hungary',
			'IS'=>'Iceland',
			'IN'=>'India',
			'ID'=>'Indonesia',
			'IR'=>'Iran, Islamic Republic of',
			'IQ'=>'Iraq',
			'IE'=>'Ireland',
			'IM'=>'Isle of Man',
			'IL'=>'Israel',
			'IT'=>'Italy',
			'JM'=>'Jamaica',
			'JP'=>'Japan',
			'JE'=>'Jersey',
			'JO'=>'Jordan',
			'KZ'=>'Kazakhstan',
			'KE'=>'Kenya',
			'KI'=>'Kiribati',
			'KP'=>'Korea, Democratic People\'s Republic of',
			'KR'=>'Korea, Republic of',
			'KW'=>'Kuwait',
			'KG'=>'Kyrgyzstan',
			'LA'=>'Lao People\'s Democratic Republic',
			'LV'=>'Latvia',
			'LB'=>'Lebanon',
			'LS'=>'Lesotho',
			'LR'=>'Liberia',
			'LY'=>'Libyan Arab Jamahiriya',
			'LI'=>'Liechtenstein',
			'LT'=>'Lithuania',
			'LU'=>'Luxembourg',
			'MO'=>'Macao',
			'MK'=>'Macedonia, The Former Yugoslav Republic of',
			'MG'=>'Madagascar',
			'MW'=>'Malawi',
			'MY'=>'Malaysia',
			'MV'=>'Maldives',
			'ML'=>'Mali',
			'MT'=>'Malta',
			'MH'=>'Marshall Islands',
			'MQ'=>'Martinique',
			'MR'=>'Mauritania',
			'MU'=>'Mauritius',
			'YT'=>'Mayotte',
			'MX'=>'Mexico',
			'FM'=>'Micronesia, Federated States of',
			'MD'=>'Moldova, Republic of',
			'MC'=>'Monaco',
			'MN'=>'Mongolia',
			'ME'=>'Montenegro',
			'MS'=>'Montserrat',
			'MA'=>'Morocco',
			'MZ'=>'Mozambique',
			'MM'=>'Myanmar',
			'NA'=>'Namibia',
			'NR'=>'Nauru',
			'NP'=>'Nepal',
			'NL'=>'Netherlands',
			'AN'=>'Netherlands Antilles',
			'NC'=>'New Caledonia',
			'NZ'=>'New Zealand',
			'NI'=>'Nicaragua',
			'NE'=>'Niger',
			'NG'=>'Nigeria',
			'NU'=>'Niue',
			'NF'=>'Norfolk Island',
			'MP'=>'Northern Mariana Islands',
			'NO'=>'Norway',
			'OM'=>'Oman',
			'PK'=>'Pakistan',
			'PW'=>'Palau',
			'PS'=>'Palestinian Territory, Occupied',
			'PA'=>'Panama',
			'PG'=>'Papua New Guinea',
			'PY'=>'Paraguay',
			'PE'=>'Peru',
			'PH'=>'Philippines',
			'PN'=>'Pitcairn',
			'PL'=>'Poland',
			'PT'=>'Portugal',
			'PR'=>'Puerto Rico',
			'QA'=>'Qatar',
			'RE'=>'Reunion',
			'RO'=>'Romania',
			'RU'=>'Russian Federation',
			'RW'=>'Rwanda',
			'SH'=>'Saint Helena',
			'KN'=>'Saint Kitts and Nevis',
			'LC'=>'Saint Lucia',
			'PM'=>'Saint Pierre and Miquelon',
			'VC'=>'Saint Vincent and The Grenadines',
			'WS'=>'Samoa',
			'SM'=>'San Marino',
			'ST'=>'Sao Tome and Principe',
			'SA'=>'Saudi Arabia',
			'SN'=>'Senegal',
			'RS'=>'Serbia',
			'SC'=>'Seychelles',
			'SL'=>'Sierra Leone',
			'SG'=>'Singapore',
			'SK'=>'Slovakia',
			'SI'=>'Slovenia',
			'SB'=>'Solomon Islands',
			'SO'=>'Somalia',
			'ZA'=>'South Africa',
			'GS'=>'South Georgia and The South Sandwich Islands',
			'ES'=>'Spain',
			'LK'=>'Sri Lanka',
			'SD'=>'Sudan',
			'SR'=>'Suriname',
			'SJ'=>'Svalbard and Jan Mayen',
			'SZ'=>'Swaziland',
			'SE'=>'Sweden',
			'CH'=>'Switzerland',
			'SY'=>'Syrian Arab Republic',
			'TW'=>'Taiwan, Province of China',
			'TJ'=>'Tajikistan',
			'TZ'=>'Tanzania, United Republic of',
			'TH'=>'Thailand',
			'TL'=>'Timor-leste',
			'TG'=>'Togo',
			'TK'=>'Tokelau',
			'TO'=>'Tonga',
			'TT'=>'Trinidad and Tobago',
			'TN'=>'Tunisia',
			'TR'=>'Turkey',
			'TM'=>'Turkmenistan',
			'TC'=>'Turks and Caicos Islands',
			'TV'=>'Tuvalu',
			'UG'=>'Uganda',
			'UA'=>'Ukraine',
			'AE'=>'United Arab Emirates',
			'GB'=>'United Kingdom',
			'US'=>'United States',
			'UM'=>'United States Minor Outlying Islands',
			'UY'=>'Uruguay',
			'UZ'=>'Uzbekistan',
			'VU'=>'Vanuatu',
			'VE'=>'Venezuela',
			'VN'=>'Viet Nam',
			'VG'=>'Virgin Islands, British',
			'VI'=>'Virgin Islands, U.S.',
			'WF'=>'Wallis and Futuna',
			'EH'=>'Western Sahara',
			'YE'=>'Yemen',
			'ZM'=>'Zambia',
			'ZW'=>'Zimbabwe',
		);
		
		if(array_key_exists($code, $map))
		{
			return $map[$code];
		}
		
		return 'Unknown';
	}
}