import type { PageServerLoad } from "./$types";

export const load: PageServerLoad = async ({ depends, locals: { supabase, user } }) => {
  depends('supabase:db:devices');
  depends('supabase:db:profiles');
  depends('supabase:db:actions');

  const { data } = await supabase
    .from('devices')
    .select()
    .eq('user', user?.id)
    .is('connected', true)
    .order('name', { ascending: true });

  return { devices: data ?? [] };
};