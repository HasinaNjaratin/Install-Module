<?php

define('MODULE_CONTRIB_DIRECTORY', variable_get('module_contrib_directory','sites/all/modules/contrib'));

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
 * Function to download external module and extract it in module directory
 *
 * @param string $modulename
*/
function getExternalModuleUrl($modulename) {
  $module_info = getExternalModuleInfo($modulename);
  if(!empty($module_info)){
  	$module_download_url = $array['releases']['release'][0]['download_link'];
  	return $module_download_url;
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
 * Function inspired from update_manager_archive_extract($file, $directory)
 * extracts a file to a directory
 *
 * @param path_to_file $file
 * @param path_to_directory $directory
 * @throws Exception
 * @return A path
 */
function extractFile($file, $directory) {
  $archiver = archiver_get_archiver($file);
  if (!$archiver) {
    throw new Exception(t('Cannot extract %file, not a valid archive.', array(
      '%file' => $file
    )));
  }

  // Remove the directory if it exists, otherwise it might contain a mixture of
  // old files mixed with the new files (e.g. in cases where files were removed
  // from a later release).
  $files = $archiver->listContents();

  // Unfortunately, we can only use the directory name to determine the project
  // name. Some archivers list the first file as the directory (i.e., MODULE/)
  // and others list an actual file (i.e., MODULE/README.TXT).
  $project = strtok($files[0], '/\\');

  $extract_location = $directory . '/' . $project;
  if (file_exists($extract_location)) {
    file_unmanaged_delete_recursive($extract_location);
  }

  $archiver->extract($directory);
  return $archiver;
}

function installExternalModule($modulename){
	$file = downloadExternalmodule($modulename);
	if($file){
  	extractFile($file, MY_MODULE_MODULE_DIRECTORY);
	}else{
		drupal_set_message($modulename.'is not avalaible','error');
	}
}