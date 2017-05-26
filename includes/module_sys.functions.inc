<?php

define('MODULE_CONTRIB_DIRECTORY', variable_get('module_contrib_directory','sites/all/modules/contrib'));

/**
 *
 * Function to get info of internal module which is not enable
 * @param string $modulename
*/

function getInternalModuleInfo($modulename){
  $getInfo = array();
  $file = new SPLFileObject(drupal_get_path('module',$modulename).'/'.$modulename.'.info');
  try{
    foreach($file as $line) {
      $getInfo[] = htmlspecialchars($line);
    }
  }catch(Exception $e){}
  if(!empty($getInfo)){
  	$info = array();
  	$tmpkeyInfo = '';
  	$debutMultiple = TRUE;
  	foreach ($getInfo as $key => $value) {
  		if(strpos($value, '=') !== FALSE){
  			$keyInfo = trim(strstr($value, '=', true));
  			$valueInfo = strstr($value, '=');
  			if(strpos($keyInfo, '[]') !== FALSE){
  				$info[trim($keyInfo,'[]')][] = ltrim($valueInfo,'=');
  			}else{
  				$info[$keyInfo] = ltrim($valueInfo,'=');
  			}
  		}
  	}
  	return $info;
  }else{
  	return FALSE;
  }
}


/**
 *
 * Function to get external module info
 *
 * @param string $modulename
*/
function getExternalModuleInfo($modulename){
	$module_info = array();
	$drupal_get_module_url = 'http://updates.drupal.org/release-history/' . $modulename . '/7.x';
  $xml = drupal_http_request($drupal_get_module_url);
  if (!isset($xml->error) && isset($xml->data)) {
    $data = $xml->data;
  }
  try {$xml = new SimpleXMLElement($data);}
  catch (Exception $e) {
    return;
  }
  $json = json_encode($xml);
  $module_info = json_decode($json, TRUE);
  return $module_info;
}


/**
 *
 * get url of external module to download it
 *
 * @param string $modulename
*/
function getExternalModuleUrl($modulename) {
  $module_info = getExternalModuleInfo($modulename);
  if(!empty($module_info)){
  	if(isset($module_info['releases']['release'][0]['download_link'])){
  		$module_download_url = $module_info['releases']['release'][0]['download_link'];
	  	return $module_download_url;
  	}else{
  		return FALSE;
  	}
  }else{
  	return FALSE;
  }
}

/**
 *
 * @param url $url
 */
function downloadExternalmodule($modulename) {
	$moduleUrl = getExternalModuleUrl($modulename);
	if($moduleUrl){
		$parsed_url = parse_url($moduleUrl);
	  $remote_schemes = array(
	    'http',
	    'https',
	    'ftp',
	    'ftps',
	    'smb',
	    'nfs'
	  );
	  if (!in_array($parsed_url['scheme'], $remote_schemes)) {
	    return drupal_realpath($moduleUrl);
	  }
	  $cache_directory = _update_manager_cache_directory();
	  $local = $cache_directory . '/' . drupal_basename($parsed_url['path']);
	  return system_retrieve_file($moduleUrl, $local, FALSE, FILE_EXISTS_REPLACE);
	}else{
		return FALSE;
	}
}

/**
 * 
 * extracts a file to a directory
 *
 */
function extractFile($file, $directory) {
  $archiver = archiver_get_archiver($file);
  if (!$archiver) {
    throw new Exception(t('Cannot extract %file, not a valid archive.', array(
      '%file' => $file
    )));
  }
  $files = $archiver->listContents();
  $project = strtok($files[0], '/\\');
  $extract_location = $directory . '/' . $project;
  if (file_exists($extract_location)) {
    file_unmanaged_delete_recursive($extract_location);
  }
  $archiver->extract($directory);
  return $archiver;
}

function putExternalModule($modulename){
	$file = downloadExternalmodule($modulename);
	if($file){
  	extractFile($file, MODULE_CONTRIB_DIRECTORY);
	}else{
		drupal_set_message($modulename.'is not avalaible','error');
	}
}

function installModule($modulename){
	/*Check if module repository exist*/
	if(!in_array($modulename, module_list())){
		putExternalModule($modulename);
	}
	$module_info = getInternalModuleInfo($modulename);
	if(!empty($module_info['dependencies'])){
		foreach ($module_info['dependencies'] as $key => $value) {
			if(!in_array($value, module_list())){
				$aValue = explode(" ", ltrim($value,' '));
				$depedency_name = $aValue[0];
				putExternalModule($depedency_name);
			}
		}
	}
	module_enable(array(trim($modulename)), FALSE);
	drupal_set_message($modulename.' has been installed successfully','status');
}