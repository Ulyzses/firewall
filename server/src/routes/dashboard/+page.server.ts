import type { PageServerLoad } from "./$types";

export const load: PageServerLoad = async ({ depends, locals: { supabase } }) => {
  depends('supabase:db:devices');
  depends('supabase:db:profiles');
  depends('supabase:db:actions');

  const { data } = await supabase
    .from('devices')
    .select()
    .order('name', { ascending: true });

  return { devices: data ?? [] };
};