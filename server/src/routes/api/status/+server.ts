import { text } from '@sveltejs/kit';
import type { RequestHandler } from '@sveltejs/kit';

export const GET: RequestHandler = async ({ url, locals: { supabase } }) => {
  const macAddress = url.searchParams.get('m') as string;

  if (!macAddress) {
    return text('MAC Address is required', { status: 400 });
  }

  if (!/^([0-9a-fA-F]{2}\:){5}[0-9a-fA-F]{2}$/.test(macAddress)) {
    return text('Invalid MAC Address', { status: 400 });
  }

  const { data, error } = await supabase
    .from('devices')
    .select()
    .eq('mac', macAddress)
    .is('connected', true)
    .maybeSingle();

  console.log(data, error);
  
  if (error) {
    return text(`Error: ${error.message}`, { status: 500 });
  }

  if (!data) {
    return text('Device not found', { status: 404 });
  }

  return text(`Server is up and running. MAC Address: ${macAddress}`);
}