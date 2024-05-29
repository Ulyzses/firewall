import { text } from '@sveltejs/kit';
import type { RequestHandler } from '@sveltejs/kit';

export const GET: RequestHandler = async ({ url }) => {
  const macAddress = url.searchParams.get('m') as string;

  return text(`Server is up and running. MAC Address: ${macAddress}`);
}